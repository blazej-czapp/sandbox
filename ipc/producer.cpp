#include "ring_buffer.hpp"

#include <cstring> // strncpy
#include <fcntl.h> // O_CREAT, O_RDWR
#include <iostream>
#include <print>
#include <sys/mman.h> // mmap, munmap, PROT_*, MAP_*
#include <unistd.h>   // ftruncate, close

using Buffer = SPSCRingBuffer<int>;

int main() {
    // create a shared memory object (analogous to e.g. a file)
    // O_CREAT - create if doesn't exist
    // O_RDWR - read/write
    // 0600 - owner can read/write
    const int fd = shm_open("/my_channel", O_CREAT | O_RDWR, 0600);

    // by default the object has length 0
    ftruncate(fd, sizeof(Buffer));


    // map the object into the process' address space
    void *ptr = mmap(nullptr,                // let the OS pick the virtual address
                     sizeof(Buffer),         // how many bytes to map
                     PROT_READ | PROT_WRITE, // we want to read and write through this mapping
                     MAP_SHARED, // changes are visible to other processes (vs MAP_PRIVATE)
                     fd,         // which object to map
                     0           // offset into the object (start from byte 0)
    );

    // When you mmap a file with MAP_PRIVATE, the OS maps the file's pages into your address space —
    // but makes a promise: your writes will never touch the original file.

    // It doesn't immediately make a copy of the file. That would be wasteful if you only write to
    // one page out of a thousand. Instead it waits. The moment you write to a page, the OS:

    // 1. Allocates a fresh physical page
    // 2. Copies the original page's contents into it
    // 3. Rewires your page table entry to point at the new page
    // 4. Lets your write land there

    // The most common use case: loading executables and shared libraries.

    // When you run a program, the OS uses MAP_PRIVATE to map the binary's code and read-only data.
    // If 50 instances of the same program run at once, they all share the same physical pages for
    // the code (nobody writes to it, so no copies are made).

    // Another use: zero-overhead file reading. You can mmap a large file MAP_PRIVATE | MAP_POPULATE
    // and treat it as a read-only byte array, without any read() calls. The kernel pages in only
    // what you touch. Fintech uses this for reading large reference data files (e.g. instrument
    // static data) at startup.

    // Once mmap succeeds, the mapping is independent of the file descriptor.
    // The physical pages stay alive as long as any mapping exists. Closing fd
    // here is clean hygiene - it doesn't unmap anything.
    close(fd);

    Buffer* buf = new (ptr) Buffer;

    // silly copy directly into the buffer - not synchronized so it's UB if anyone is reading
    // char *msg = static_cast<char *>(ptr);
    // std::strncpy(msg, "hello", 64);
    // std::println("wrote: {}", msg);

    // cool :)
    // $ cat /dev/shm/my_channel
    // hello⏎                                                                                                                                                                                                                                        ~/w/ipc $ file /dev/shm/my_channel
    // $ file /dev/shm/my_channel
    // /dev/shm/my_channel: data

    buf->write(42);
    std::println("wrote 42");

    std::cin.get(); // pause so the consumer can connect before we clean up

    buf->~Buffer();
    munmap(ptr, Buffer::size); // remove the mapping from this process's address space
                               // curious that size is a required param
    shm_unlink("/my_channel"); // delete the named object from the kernel
}
