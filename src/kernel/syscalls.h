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
    SYS_sigaltstack        = 53,
    SYS_ioctl              = 54,
    SYS_reboot             = 55,
    SYS_revoke             = 56,
    SYS_symlink            = 57,
    SYS_readlink           = 58,
    SYS_execve             = 59,
    SYS_umask              = 60,
    SYS_chroot             = 61,
    SYS_compat_fstat       = 62,
    SYS_compat_getkerninfo = 63,
    SYS_compat_getpagesize = 64,
    SYS_msync              = 65,
    SYS_vfork              = 66,
    SYS_obs_vread          = 67,
    SYS_obs_vwrite         = 68,
    SYS_sbrk               = 69,
    SYS_sstk               = 70,
    SYS_compat_mmap        = 71,
    SYS_obsolete72         = 72,
    SYS_munmap             = 73,
    SYS_mprotect           = 74,
    SYS_madvise            = 75,
    SYS_obs_vhangup        = 76,
    SYS_obs_vlimit         = 77,
    SYS_mincore            = 78,
    SYS_getgroups          = 79,
    SYS_setgroups          = 80,
    SYS_getpgrp            = 81,
    SYS_setpgid            = 82,
    SYS_setitimer          = 83,
    SYS_compat_wait        = 84,
    SYS_swapon             = 85,
    SYS_getitimer          = 86,
    SYS_compat_gethostname = 87,
    SYS_compat_sethostname = 88,
    SYS_getdtablesize      = 89,
    SYS_dup2               = 90,
    SYS_number91           = 91,
    SYS_fcntl              = 92,
    SYS_select             = 93,
    SYS_number94           = 94,
    SYS_fsync              = 95,
    SYS_setpriority        = 96,
    SYS_socket             = 97,
    SYS_connect            = 98,
    SYS_netcontrol         = 99,
    SYS_getpriority        = 100,
    SYS_netabort           = 101,
    SYS_netgetsockinfo     = 102,
    SYS_compat_sigreturn   = 103,
    SYS_bind               = 104,
    SYS_setsockopt         = 105,
    SYS_listen             = 106,
    SYS_obs_vtimes         = 107,
    SYS_compat_sigvec      = 108,
    SYS_compat_sigblock    = 109,
    SYS_compat_sigsetmask  = 110,
    SYS_compat_sigsuspend  = 111,
    SYS_compat_sigstack    = 112,
    SYS_socketex           = 113,
    SYS_socketclose        = 114,
    SYS_obs_vtrace         = 115,
    SYS_gettimeofday       = 116,
    SYS_getrusage          = 117,
    SYS_getsockopt         = 118,
    SYS_number119          = 119,
    SYS_readv              = 120,
    SYS_writev             = 121,
    SYS_settimeofday       = 122,
    SYS_fchown             = 123,
    SYS_fchmod             = 124,
    SYS_netgetiflist       = 125,
    SYS_setreuid           = 126,
    SYS_setregid           = 127,
    SYS_rename             = 128,
    SYS_compat_truncate    = 129,
    SYS_compat_ftruncate   = 130,
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
