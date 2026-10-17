#include "syscalls.h"

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <fcntl.h>
#  include <signal.h>
#  include <sys/socket.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#  include <sys/wait.h>
#  include <unistd.h>
#endif

namespace maure::kernel {

namespace {

#if defined(_WIN32)
constexpr int kMaxFd = 1024;
HANDLE g_fds[kMaxFd] = {};
bool g_fd_used[kMaxFd] = {};

int alloc_fd(HANDLE h) {
    for (int i = 3; i < kMaxFd; ++i) {
        if (!g_fd_used[i]) {
            g_fd_used[i] = true;
            g_fds[i] = h;
            return i;
        }
    }
    return -1;
}

HANDLE fd_to_handle(int fd) {
    if (fd < 0 || fd >= kMaxFd || !g_fd_used[fd])
        return INVALID_HANDLE_VALUE;
    return g_fds[fd];
}

void free_fd(int fd) {
    if (fd >= 0 && fd < kMaxFd && g_fd_used[fd]) {
        g_fd_used[fd] = false;
        g_fds[fd] = nullptr;
    }
}

int64_t win_err() {
    switch (GetLastError()) {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
        return -ENOENT;
    case ERROR_ACCESS_DENIED:
        return -EACCES;
    case ERROR_INVALID_HANDLE:
        return -EBADF;
    case ERROR_ALREADY_EXISTS:
        return -EEXIST;
    default:
        return -EIO;
    }
}
#endif

bool read_guest(SyscallContext& ctx, uint64_t va, void* dst, size_t len) {
    if (!ctx.mem || va == 0)
        return false;
    if (void* p = ctx.mem->try_host_ptr(va, len)) {
        std::memcpy(dst, p, len);
        return true;
    }
    return ctx.mem->read(va, dst, len);
}

bool write_guest(SyscallContext& ctx, uint64_t va, const void* src, size_t len) {
    if (!ctx.mem || va == 0)
        return false;
    if (void* p = ctx.mem->try_host_ptr(va, len)) {
        std::memcpy(p, src, len);
        return true;
    }
    return ctx.mem->write(va, src, len);
}

std::string read_guest_string(SyscallContext& ctx, uint64_t va, size_t max_len = 4096) {
    std::string out;
    if (!ctx.mem || va == 0)
        return out;
    out.resize(max_len);
    size_t got = 0;
    for (; got < max_len; ++got) {
        char c = 0;
        if (!read_guest(ctx, va + got, &c, 1))
            break;
        if (c == '\0')
            break;
        out[got] = c;
    }
    out.resize(got);
    return out;
}

std::string host_path(SyscallContext& ctx, const std::string& guest_path) {
    if (guest_path.empty())
        return guest_path;
    if (!ctx.rootfs.empty() && guest_path[0] == '/')
        return ctx.rootfs + guest_path;
    return guest_path;
}

int64_t sys_exit(SyscallContext& ctx, uint64_t code) {
    if (ctx.cpu)
        ctx.cpu->halt = true;
#if defined(_WIN32)
    (void)code;
#else
    (void)code;
#endif
    return static_cast<int64_t>(code);
}

int64_t sys_fork(SyscallContext&) {
    return -ENOSYS;
}

int64_t sys_read(SyscallContext& ctx, uint64_t fd, uint64_t buf, uint64_t nbytes) {
    if (!buf)
        return -EFAULT;
    const size_t n = static_cast<size_t>(nbytes);
    std::vector<uint8_t> tmp(n);

#if defined(_WIN32)
    HANDLE h = (fd == 0) ? GetStdHandle(STD_INPUT_HANDLE) : fd_to_handle(static_cast<int>(fd));
    if (h == INVALID_HANDLE_VALUE)
        return -EBADF;
    DWORD got = 0;
    if (!ReadFile(h, tmp.data(), static_cast<DWORD>(n), &got, nullptr))
        return win_err();
    if (!write_guest(ctx, buf, tmp.data(), got))
        return -EFAULT;
    return static_cast<int64_t>(got);
#else
    ssize_t r = ::read(static_cast<int>(fd), tmp.data(), n);
    if (r < 0)
        return -errno;
    if (r > 0 && !write_guest(ctx, buf, tmp.data(), static_cast<size_t>(r)))
        return -EFAULT;
    return static_cast<int64_t>(r);
#endif
}

int64_t sys_write(SyscallContext& ctx, uint64_t fd, uint64_t buf, uint64_t nbytes) {
    if (!buf)
        return -EFAULT;
    const size_t n = static_cast<size_t>(nbytes);
    std::vector<uint8_t> tmp(n);
    if (!read_guest(ctx, buf, tmp.data(), n))
        return -EFAULT;

#if defined(_WIN32)
    HANDLE h = INVALID_HANDLE_VALUE;
    if (fd == 1)
        h = GetStdHandle(STD_OUTPUT_HANDLE);
    else if (fd == 2)
        h = GetStdHandle(STD_ERROR_HANDLE);
    else
        h = fd_to_handle(static_cast<int>(fd));
    if (h == INVALID_HANDLE_VALUE)
        return -EBADF;
    DWORD wrote = 0;
    if (!WriteFile(h, tmp.data(), static_cast<DWORD>(n), &wrote, nullptr))
        return win_err();
    return static_cast<int64_t>(wrote);
#else
    ssize_t r = ::write(static_cast<int>(fd), tmp.data(), n);
    if (r < 0)
        return -errno;
    return static_cast<int64_t>(r);
#endif
}

int64_t sys_open(SyscallContext& ctx, uint64_t path_va, uint64_t flags, uint64_t mode) {
    std::string path = read_guest_string(ctx, path_va);
    if (path.empty())
        return -EFAULT;
    path = host_path(ctx, path);
    (void)mode;

#if defined(_WIN32)
    DWORD access = GENERIC_READ;
    DWORD disp = OPEN_EXISTING;
    if (flags & 1ull)
        access = GENERIC_WRITE;
    if ((flags & 3ull) == 2ull)
        access = GENERIC_READ | GENERIC_WRITE;
    if (flags & 0x200ull)
        disp = OPEN_ALWAYS;
    HANDLE h = CreateFileA(path.c_str(), access, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, disp,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE)
        return win_err();
    int fd = alloc_fd(h);
    if (fd < 0) {
        CloseHandle(h);
        return -EMFILE;
    }
    return fd;
#else
    int fd = ::open(path.c_str(), static_cast<int>(flags), static_cast<mode_t>(mode));
    if (fd < 0)
        return -errno;
    return fd;
#endif
}

int64_t sys_close(SyscallContext&, uint64_t fd) {
#if defined(_WIN32)
    if (fd <= 2)
        return 0;
    HANDLE h = fd_to_handle(static_cast<int>(fd));
    if (h == INVALID_HANDLE_VALUE)
        return -EBADF;
    CloseHandle(h);
    free_fd(static_cast<int>(fd));
    return 0;
#else
    if (::close(static_cast<int>(fd)) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_wait4(SyscallContext&, uint64_t, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_creat(SyscallContext& ctx, uint64_t path_va, uint64_t mode) {
    std::string path = host_path(ctx, read_guest_string(ctx, path_va));
    if (path.empty())
        return -EFAULT;
#if defined(_WIN32)
    (void)mode;
    HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE)
        return win_err();
    int fd = alloc_fd(h);
    if (fd < 0) {
        CloseHandle(h);
        return -EMFILE;
    }
    return fd;
#else
    int fd = ::creat(path.c_str(), static_cast<mode_t>(mode));
    if (fd < 0)
        return -errno;
    return fd;
#endif
}

int64_t sys_link(SyscallContext& ctx, uint64_t p1, uint64_t p2) {
    std::string a = host_path(ctx, read_guest_string(ctx, p1));
    std::string b = host_path(ctx, read_guest_string(ctx, p2));
    if (a.empty() || b.empty())
        return -EFAULT;
#if defined(_WIN32)
    if (!CreateHardLinkA(b.c_str(), a.c_str(), nullptr))
        return win_err();
    return 0;
#else
    if (::link(a.c_str(), b.c_str()) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_unlink(SyscallContext& ctx, uint64_t path_va) {
    std::string path = host_path(ctx, read_guest_string(ctx, path_va));
    if (path.empty())
        return -EFAULT;
#if defined(_WIN32)
    if (!DeleteFileA(path.c_str()))
        return win_err();
    return 0;
#else
    if (::unlink(path.c_str()) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_obs_execv(SyscallContext&, uint64_t, uint64_t) { return -ENOSYS; }

int64_t sys_chdir(SyscallContext& ctx, uint64_t path_va) {
    std::string path = host_path(ctx, read_guest_string(ctx, path_va));
    if (path.empty())
        return -EFAULT;
#if defined(_WIN32)
    if (!SetCurrentDirectoryA(path.c_str()))
        return win_err();
    return 0;
#else
    if (::chdir(path.c_str()) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_fchdir(SyscallContext&, uint64_t) { return -ENOSYS; }

int64_t sys_mknod(SyscallContext& ctx, uint64_t path_va, uint64_t mode, uint64_t dev) {
    std::string path = host_path(ctx, read_guest_string(ctx, path_va));
    if (path.empty())
        return -EFAULT;
#if defined(_WIN32)
    (void)mode;
    (void)dev;
    HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE)
        return win_err();
    CloseHandle(h);
    return 0;
#else
    if (::mknod(path.c_str(), static_cast<mode_t>(mode), static_cast<dev_t>(dev)) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_chmod(SyscallContext& ctx, uint64_t path_va, uint64_t mode) {
    std::string path = host_path(ctx, read_guest_string(ctx, path_va));
    if (path.empty())
        return -EFAULT;
#if defined(_WIN32)
    DWORD attrs = GetFileAttributesA(path.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES)
        return win_err();
    if ((mode & 0222) == 0)
        attrs |= FILE_ATTRIBUTE_READONLY;
    else
        attrs &= ~FILE_ATTRIBUTE_READONLY;
    if (!SetFileAttributesA(path.c_str(), attrs))
        return win_err();
    return 0;
#else
    if (::chmod(path.c_str(), static_cast<mode_t>(mode)) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_chown(SyscallContext& ctx, uint64_t path_va, uint64_t uid, uint64_t gid) {
    std::string path = host_path(ctx, read_guest_string(ctx, path_va));
    if (path.empty())
        return -EFAULT;
#if defined(_WIN32)
    (void)uid;
    (void)gid;
    return 0;
#else
    if (::chown(path.c_str(), static_cast<uid_t>(uid), static_cast<gid_t>(gid)) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_lseek(SyscallContext&, uint64_t fd, uint64_t offset, uint64_t whence) {
#if defined(_WIN32)
    HANDLE h = fd_to_handle(static_cast<int>(fd));
    if (h == INVALID_HANDLE_VALUE)
        return -EBADF;
    DWORD method = FILE_BEGIN;
    if (whence == 1)
        method = FILE_CURRENT;
    else if (whence == 2)
        method = FILE_END;
    LARGE_INTEGER li;
    li.QuadPart = static_cast<LONGLONG>(offset);
    LARGE_INTEGER out{};
    if (!SetFilePointerEx(h, li, &out, method))
        return win_err();
    return static_cast<int64_t>(out.QuadPart);
#else
    off_t r = ::lseek(static_cast<int>(fd), static_cast<off_t>(offset), static_cast<int>(whence));
    if (r < 0)
        return -errno;
    return static_cast<int64_t>(r);
#endif
}

int64_t sys_getpid(SyscallContext&) {
#if defined(_WIN32)
    return static_cast<int64_t>(GetCurrentProcessId());
#else
    return static_cast<int64_t>(::getpid());
#endif
}

int64_t sys_setuid(SyscallContext&, uint64_t) { return 0; }
int64_t sys_getuid(SyscallContext&) { return 0; }
int64_t sys_geteuid(SyscallContext&) { return 0; }
int64_t sys_getgid(SyscallContext&) { return 0; }
int64_t sys_getegid(SyscallContext&) { return 0; }
int64_t sys_getppid(SyscallContext&) { return 0; }

int64_t sys_access(SyscallContext& ctx, uint64_t path_va, uint64_t mode) {
    std::string path = host_path(ctx, read_guest_string(ctx, path_va));
    if (path.empty())
        return -EFAULT;
#if defined(_WIN32)
    (void)mode;
    if (GetFileAttributesA(path.c_str()) == INVALID_FILE_ATTRIBUTES)
        return win_err();
    return 0;
#else
    if (::access(path.c_str(), static_cast<int>(mode)) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_sync(SyscallContext&) {
#if !defined(_WIN32)
    ::sync();
#endif
    return 0;
}

int64_t sys_kill(SyscallContext&, uint64_t pid, uint64_t sig) {
#if defined(_WIN32)
    if (sig == 9 || sig == 15) {
        HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, static_cast<DWORD>(pid));
        if (!h)
            return win_err();
        BOOL ok = TerminateProcess(h, 1);
        CloseHandle(h);
        return ok ? 0 : win_err();
    }
    return -ENOSYS;
#else
    if (::kill(static_cast<pid_t>(pid), static_cast<int>(sig)) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_stat_like(SyscallContext& ctx, uint64_t path_va, uint64_t buf_va, bool /*lstat*/) {
    std::string path = host_path(ctx, read_guest_string(ctx, path_va));
    if (path.empty() || !buf_va)
        return -EFAULT;
#if defined(_WIN32)
    if (GetFileAttributesA(path.c_str()) == INVALID_FILE_ATTRIBUTES)
        return win_err();
    uint8_t zero[256]{};
    if (!write_guest(ctx, buf_va, zero, sizeof(zero)))
        return -EFAULT;
    return 0;
#else
    struct stat st{};
    if (::stat(path.c_str(), &st) < 0)
        return -errno;
    if (!write_guest(ctx, buf_va, &st, sizeof(st)))
        return -EFAULT;
    return 0;
#endif
}

int64_t sys_dup(SyscallContext&, uint64_t fd) {
#if defined(_WIN32)
    HANDLE h = fd_to_handle(static_cast<int>(fd));
    if (h == INVALID_HANDLE_VALUE)
        return -EBADF;
    HANDLE proc = GetCurrentProcess();
    HANDLE duped = nullptr;
    if (!DuplicateHandle(proc, h, proc, &duped, 0, FALSE, DUPLICATE_SAME_ACCESS))
        return win_err();
    int nfd = alloc_fd(duped);
    if (nfd < 0) {
        CloseHandle(duped);
        return -EMFILE;
    }
    return nfd;
#else
    int r = ::dup(static_cast<int>(fd));
    if (r < 0)
        return -errno;
    return r;
#endif
}

int64_t sys_pipe(SyscallContext& ctx, uint64_t fds_va) {
    if (!fds_va)
        return -EFAULT;
#if defined(_WIN32)
    HANDLE rd = nullptr, wr = nullptr;
    if (!CreatePipe(&rd, &wr, nullptr, 0))
        return win_err();
    int rfd = alloc_fd(rd);
    int wfd = alloc_fd(wr);
    if (rfd < 0 || wfd < 0) {
        if (rfd >= 0)
            free_fd(rfd);
        if (wfd >= 0)
            free_fd(wfd);
        CloseHandle(rd);
        CloseHandle(wr);
        return -EMFILE;
    }
    int out[2] = {rfd, wfd};
    if (!write_guest(ctx, fds_va, out, sizeof(out)))
        return -EFAULT;
    return 0;
#else
    int fds[2];
    if (::pipe(fds) < 0)
        return -errno;
    if (!write_guest(ctx, fds_va, fds, sizeof(fds)))
        return -EFAULT;
    return 0;
#endif
}

int64_t sys_getlogin(SyscallContext& ctx, uint64_t buf_va, uint64_t len) {
    if (!buf_va)
        return -EFAULT;
    char name[256] = {};
#if defined(_WIN32)
    DWORD n = sizeof(name);
    if (!GetUserNameA(name, &n))
        return win_err();
#else
    const char* login = ::getlogin();
    if (!login)
        return -ENOTTY;
    std::strncpy(name, login, sizeof(name) - 1);
#endif
    size_t copy = std::strlen(name);
    if (len && copy >= static_cast<size_t>(len))
        copy = static_cast<size_t>(len) > 0 ? static_cast<size_t>(len) - 1 : 0;
    if (!write_guest(ctx, buf_va, name, copy + 1))
        return -EFAULT;
    return 0;
}

int64_t enosys() { return -ENOSYS; }

} // namespace

int64_t dispatch_syscall(SyscallContext& ctx, uint32_t nr,
                         uint64_t a1, uint64_t a2, uint64_t a3,
                         uint64_t a4, uint64_t a5, uint64_t a6) {
    switch (nr) {
    case SYS_syscall:
        return dispatch_syscall(ctx, static_cast<uint32_t>(a1), a2, a3, a4, a5, a6, 0);
    case SYS_exit:
        return sys_exit(ctx, a1);
    case SYS_fork:
        return sys_fork(ctx);
    case SYS_read:
        return sys_read(ctx, a1, a2, a3);
    case SYS_write:
        return sys_write(ctx, a1, a2, a3);
    case SYS_open:
        return sys_open(ctx, a1, a2, a3);
    case SYS_close:
        return sys_close(ctx, a1);
    case SYS_wait4:
        return sys_wait4(ctx, a1, a2, a3, a4);
    case SYS_compat_creat:
        return sys_creat(ctx, a1, a2);
    case SYS_link:
        return sys_link(ctx, a1, a2);
    case SYS_unlink:
        return sys_unlink(ctx, a1);
    case SYS_obs_execv:
        return sys_obs_execv(ctx, a1, a2);
    case SYS_chdir:
        return sys_chdir(ctx, a1);
    case SYS_fchdir:
        return sys_fchdir(ctx, a1);
    case SYS_mknod:
        return sys_mknod(ctx, a1, a2, a3);
    case SYS_chmod:
        return sys_chmod(ctx, a1, a2);
    case SYS_chown:
        return sys_chown(ctx, a1, a2, a3);
    case SYS_obsolete17:
        return enosys();
    case SYS_compat4_getfsstat:
        return enosys();
    case SYS_compat_lseek:
        return sys_lseek(ctx, a1, a2, a3);
    case SYS_getpid:
        return sys_getpid(ctx);
    case SYS_mount:
    case SYS_unmount:
        return enosys();
    case SYS_setuid:
        return sys_setuid(ctx, a1);
    case SYS_getuid:
        return sys_getuid(ctx);
    case SYS_geteuid:
        return sys_geteuid(ctx);
    case SYS_ptrace:
        return enosys();
    case SYS_recvmsg:
    case SYS_sendmsg:
    case SYS_recvfrom:
    case SYS_accept:
    case SYS_getpeername:
    case SYS_getsockname:
        return enosys();
    case SYS_access:
        return sys_access(ctx, a1, a2);
    case SYS_chflags:
    case SYS_fchflags:
        return enosys();
    case SYS_sync:
        return sys_sync(ctx);
    case SYS_kill:
        return sys_kill(ctx, a1, a2);
    case SYS_compat_stat:
        return sys_stat_like(ctx, a1, a2, false);
    case SYS_getppid:
        return sys_getppid(ctx);
    case SYS_compat_lstat:
        return sys_stat_like(ctx, a1, a2, true);
    case SYS_dup:
        return sys_dup(ctx, a1);
    case SYS_compat10_pipe:
        return sys_pipe(ctx, a1);
    case SYS_getegid:
        return sys_getegid(ctx);
    case SYS_profil:
    case SYS_ktrace:
    case SYS_compat_sigaction:
        return enosys();
    case SYS_getgid:
        return sys_getgid(ctx);
    case SYS_compat_sigprocmask:
        return enosys();
    case SYS_getlogin:
        return sys_getlogin(ctx, a1, a2);
    case SYS_setlogin:
    case SYS_obsolete51:
    case SYS_compat_sigpending:
        return enosys();
    default:
        (void)a5;
        (void)a6;
        return -ENOSYS;
    }
}

void handle_syscall(SyscallContext& ctx) {
    if (!ctx.cpu)
        return;
    GuestCpuState& c = *ctx.cpu;
    const uint32_t nr = static_cast<uint32_t>(c.rax);
    const int64_t ret = dispatch_syscall(ctx, nr, c.rdi, c.rsi, c.rdx, c.r10, c.r8, c.r9);
    c.rax = static_cast<uint64_t>(ret);
}

} // namespace maure::kernel
