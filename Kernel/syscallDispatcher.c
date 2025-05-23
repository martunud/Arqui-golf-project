#include <stdint.h>
#include <syscalls_lib.h>

typedef uint64_t (*SyscallHandler)(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8);

static SyscallHandler syscallHandlers[] = {
    (SyscallHandler)syscall_read,   // syscall 0
    (SyscallHandler)syscall_write,  // syscall 1
    (SyscallHandler)syscall_getTime, // syscall 2
    (SyscallHandler)syscall_getRegisters, // syscall 3
    (SyscallHandler)syscall_clearScreen // syscall 4
};

#define SYSCALLS_COUNT (sizeof(syscallHandlers) / sizeof(syscallHandlers[0]))

uint64_t syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t rax) {
    if (rax >= SYSCALLS_COUNT) {
        return 0;
    }
    return syscallHandlers[rax](rdi, rsi, rdx, r10, r8);
}