#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace maure::kernel {

enum SyscallNr : uint32_t {
    SYS_syscall            = 0,
    SYS_exit               = 1,
    SYS_fork               = 2,
    SYS_read               = 3,
    SYS_write              = 4,
    SYS_open               = 5,
    SYS_close              = 6,
    SYS_wait4              = 7,
    SYS_compat_creat       = 8,
    SYS_link               = 9,
    SYS_unlink             = 10,
    SYS_obs_execv          = 11,
    SYS_chdir              = 12,
    SYS_fchdir             = 13,
    SYS_mknod              = 14,
    SYS_chmod              = 15,
    SYS_chown              = 16,
    SYS_obsolete17         = 17,
    SYS_compat4_getfsstat  = 18,
    SYS_compat_lseek       = 19,
    SYS_getpid             = 20,
    SYS_mount              = 21,
    SYS_unmount            = 22,
    SYS_setuid             = 23,
    SYS_getuid             = 24,
    SYS_geteuid            = 25,
    SYS_ptrace             = 26,
    SYS_recvmsg            = 27,
    SYS_sendmsg            = 28,
    SYS_recvfrom           = 29,
    SYS_accept             = 30,
    SYS_getpeername        = 31,
    SYS_getsockname        = 32,
    SYS_access             = 33,
    SYS_chflags            = 34,
    SYS_fchflags           = 35,
    SYS_sync               = 36,
    SYS_kill               = 37,
    SYS_compat_stat        = 38,
    SYS_getppid            = 39,
    SYS_compat_lstat       = 40,
    SYS_dup                = 41,
    SYS_compat10_pipe      = 42,
    SYS_getegid            = 43,
    SYS_profil             = 44,
    SYS_ktrace             = 45,
    SYS_compat_sigaction   = 46,
    SYS_getgid             = 47,
    SYS_compat_sigprocmask = 48,
    SYS_getlogin           = 49,
    SYS_setlogin           = 50,
    SYS_obsolete51         = 51,
    SYS_compat_sigpending  = 52,
};

// Guest memory access — implement with your MMU / page tables.
struct GuestMemory {
    virtual ~GuestMemory() = default;
    virtual bool read(uint64_t va, void* host_dst, size_t len) = 0;
    virtual bool write(uint64_t va, const void* host_src, size_t len) = 0;
    // Optional: resolve VA to host pointer if you use flat identity mapping.
    virtual void* try_host_ptr(uint64_t va, size_t len) {
        (void)va;
        (void)len;
        return nullptr;
    }
};

// Register file slice used by the dispatcher (x86-64 FreeBSD/PS5 style).
struct GuestCpuState {
    uint64_t rax = 0; // syscall number on entry; return value on exit
    uint64_t rdi = 0; // arg0
    uint64_t rsi = 0; // arg1
    uint64_t rdx = 0; // arg2
    uint64_t r10 = 0; // arg3 (rcx clobbered by syscall insn)
    uint64_t r8  = 0; // arg4
    uint64_t r9  = 0; // arg5
    uint64_t rip = 0;
    bool halt = false; // set by sys_exit
};

struct SyscallContext {
    GuestMemory* mem = nullptr;
    GuestCpuState* cpu = nullptr;
    // Host path prefix for guest absolute paths (e.g. game install dir).
    std::string rootfs;
};

// Call from your CPU when the guest executes syscall / syscall-like entry.
// Reads number+args from cpu, writes result to cpu->rax, may set cpu->halt.
void handle_syscall(SyscallContext& ctx);

// Direct dispatch (tests / tooling).
int64_t dispatch_syscall(SyscallContext& ctx, uint32_t nr,
                         uint64_t a1 = 0, uint64_t a2 = 0, uint64_t a3 = 0,
                         uint64_t a4 = 0, uint64_t a5 = 0, uint64_t a6 = 0);

} // namespace maure::kernel
