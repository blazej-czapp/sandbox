#include "ring_buffer.hpp"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <print>

using Buffer = SPSCRingBuffer<int>;

int main() {
    int fd = shm_open("/my_channel", O_RDWR, 0); // O_RDWR since read() is non-const on Buffer (it needs
                                                 // to update the tail index)

    // No O_CREAT — the object must already exist. If the producer hasn't run yet, this returns
    // O_RDONLY — we only need to read. The 0 for permissions is ignored when not creating.

    void* ptr = mmap(
        nullptr,
        sizeof(Buffer),
        PROT_READ | PROT_WRITE,    // PROT_READ read-only — matches O_RDONLY above
        MAP_SHARED,
        fd,
        0
    );

    close(fd);

    // PROT_READ only — if you accidentally write through this pointer the OS will send your
    // process SIGSEGV. That's intentional protection. You can't request more permissions here
    // than you opened fd with — asking for PROT_WRITE on a O_RDONLY fd would fail.
    // With a ring buffer, we need to be able to write too, so switched to READ|WRITE

    // silly, unsynchronized read
    // const char* msg = static_cast<const char*>(ptr);
    // std::println("read: {}", msg);

    // TODO check why this isn't a reinterpret_cast
    Buffer* buf = static_cast<Buffer*>(ptr);
    std::optional<int> val;
    while (!(val = buf->read())) {}
    std::println("Read: {}", val.value());

    munmap(ptr, 64);
    // Note: no shm_unlink — the producer owns the lifetime of the object
    // The consumer doesn't call shm_unlink. Only the process responsible for the channel's
    // lifecycle should do that — otherwise the consumer could delete it while the producer is still
    // using it.
}

