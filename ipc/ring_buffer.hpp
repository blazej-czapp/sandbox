#pragma once

#include <atomic>
#include <optional>

/*!
 * Single Producer, Single Consumer - allows us to do away with locks.
 */
template <typename T, size_t LEN=64>
struct SPSCRingBuffer {
    static constexpr size_t len = LEN;
    static constexpr size_t size = LEN * sizeof(T);

private:
    T buffer[LEN];
    // alignas(64) prevent false sharing by forcing each index to go on a separate cache line
    alignas(64) std::atomic<size_t> m_tail{0}; // start of currently written region
    alignas(64) std::atomic<size_t> m_head{0}; // one past the end of currently written region

    // otherwise there's a per-process lock involved which means we can't do IPC
    static_assert(decltype(m_tail)::is_always_lock_free);
    static_assert(decltype(m_head)::is_always_lock_free);

public:
    bool write(const T& value) {
        // head only written by the writer so relaxed is enough in write()
        const auto head = m_head.load(std::memory_order_relaxed);

        // as for tail - buffer writing CANNOT be reordered before the check - can't speculatively write
        // memory, but still doing acquire to be nice and do a formal happens-before handover (I don't see
        // why it couldn't be relaxed otherwise in practice)
        const auto tail = m_tail.load(std::memory_order_acquire);
        if (incr(head) != tail) {
            buffer[head] = value;
            m_head.store(incr(head), std::memory_order_release);
            return true;
        } else {
            return false;
        }
    }

    std::optional<T> read() {
        // head needs load(acquire) to make sure buffer read doesn't get reordered before head is loaded,
        // (it can happen speculatively), which would risk reading the data while it's being written,
        // which is UB (plus it'd be stale anyway)
        // tail is only written by the reader so relaxed is enough in read()
        const auto tail = m_tail.load(std::memory_order_relaxed);
        if (m_head.load(std::memory_order_acquire) == tail) {
            return std::nullopt;
        } else {
            std::optional<T> ret{buffer[tail]};
            m_tail.store(incr(tail), std::memory_order_release);
            return ret;
        }
    }

    // can't expose these without extraneous memory ordering - not knowing know what memory
    // access happens around these calls, we don't know how relaxed the memory ordering can be
    // just call read()/write() and see if you get a success or failure
    // also, even though this is a SPSC queue, let's not expose a TOCTOU — time-of-check-time-of-use
    // issue where we check first and then attempt to write/read, _conceptually_ allowing another thread
    // to undo the condition. read() and write() already indicate success/failure so it's superflous anyway.

    // bool isFull() const {
    //     return m_head.load(std::memory_order_relaxed) == incr(m_tail.load(std::memory_order_relaxed));
    // }
    // bool hasData() const {
    //     return m_head.load(std::memory_order_relaxed) != m_tail.load(std::memory_order_relaxed);
    // }

private:
    size_t incr(size_t idx) const {
        return convertIndex(idx + 1);
    }

    // NOTE: because we wrap around, we actually lose one storage slot (write() can't push the head index
    //       all the way to overlap with tail because we treat that as empty buffer, not full)
    size_t convertIndex(size_t idx) const {
        return idx % LEN;
    }
};
