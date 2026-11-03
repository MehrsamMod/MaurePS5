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


int64_t sys_sigaltstack(SyscallContext&, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_ioctl(SyscallContext&, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_reboot(SyscallContext&, uint64_t) {
    return -ENOSYS;
}

int64_t sys_revoke(SyscallContext& ctx, uint64_t path_va) {
    (void)ctx;
    (void)path_va;
    return -ENOSYS;
}

int64_t sys_symlink(SyscallContext& ctx, uint64_t target_va, uint64_t link_va) {
    std::string target = read_guest_string(ctx, target_va);
    std::string linkpath = host_path(ctx, read_guest_string(ctx, link_va));
    if (target.empty() || linkpath.empty())
        return -EFAULT;
#if defined(_WIN32)
    // Prefer symbolic link; may require privilege — fall back ENOSYS on failure path details
    if (CreateSymbolicLinkA(linkpath.c_str(), target.c_str(), 0))
        return 0;
    return win_err();
#else
    if (::symlink(target.c_str(), linkpath.c_str()) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_readlink(SyscallContext& ctx, uint64_t path_va, uint64_t buf_va, uint64_t bufsiz) {
    std::string path = host_path(ctx, read_guest_string(ctx, path_va));
    if (path.empty() || !buf_va || !bufsiz)
        return -EFAULT;
#if defined(_WIN32)
    (void)path;
    return -ENOSYS;
#else
    std::vector<char> tmp(static_cast<size_t>(bufsiz));
    ssize_t r = ::readlink(path.c_str(), tmp.data(), tmp.size());
    if (r < 0)
        return -errno;
    if (!write_guest(ctx, buf_va, tmp.data(), static_cast<size_t>(r)))
        return -EFAULT;
    return static_cast<int64_t>(r);
#endif
}

int64_t sys_execve(SyscallContext&, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_umask(SyscallContext&, uint64_t mask) {
#if defined(_WIN32)
    (void)mask;
    return 0;
#else
    return static_cast<int64_t>(::umask(static_cast<mode_t>(mask)));
#endif
}

int64_t sys_chroot(SyscallContext& ctx, uint64_t path_va) {
    std::string path = host_path(ctx, read_guest_string(ctx, path_va));
    if (path.empty())
        return -EFAULT;
#if defined(_WIN32)
    (void)path;
    return -ENOSYS;
#else
    if (::chroot(path.c_str()) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_fstat(SyscallContext& ctx, uint64_t fd, uint64_t buf_va) {
    if (!buf_va)
        return -EFAULT;
#if defined(_WIN32)
    (void)fd;
    uint8_t zero[256]{};
    if (!write_guest(ctx, buf_va, zero, sizeof(zero)))
        return -EFAULT;
    return 0;
#else
    struct stat st{};
    if (::fstat(static_cast<int>(fd), &st) < 0)
        return -errno;
    if (!write_guest(ctx, buf_va, &st, sizeof(st)))
        return -EFAULT;
    return 0;
#endif
}

int64_t sys_getkerninfo(SyscallContext&, uint64_t, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_getpagesize(SyscallContext&) {
    return 4096;
}


int64_t sys_msync(SyscallContext& ctx, uint64_t addr, uint64_t len, uint64_t flags) {
    (void)ctx;
    (void)addr;
    (void)len;
    (void)flags;
    // No host backing required for simple guest RAM; treat as success.
    return 0;
}

int64_t sys_vfork(SyscallContext&) {
    return -ENOSYS;
}

int64_t sys_obs_vread(SyscallContext&) { return -ENOSYS; }
int64_t sys_obs_vwrite(SyscallContext&) { return -ENOSYS; }

int64_t sys_sbrk(SyscallContext&, uint64_t) {
    return -ENOSYS;
}

int64_t sys_sstk(SyscallContext&, uint64_t) {
    return -ENOSYS;
}

int64_t sys_mmap(SyscallContext& ctx, uint64_t addr, uint64_t len, uint64_t prot,
                 uint64_t flags, uint64_t fd, uint64_t pos) {
    (void)ctx;
    (void)addr;
    (void)len;
    (void)prot;
    (void)flags;
    (void)fd;
    (void)pos;
    // Real emulator should allocate guest VA via memory manager.
    // Return ENOSYS until your Memory::map is wired.
    return -ENOSYS;
}

int64_t sys_munmap(SyscallContext& ctx, uint64_t addr, uint64_t len) {
    (void)ctx;
    (void)addr;
    (void)len;
    return -ENOSYS;
}

int64_t sys_mprotect(SyscallContext& ctx, uint64_t addr, uint64_t len, uint64_t prot) {
    (void)ctx;
    (void)addr;
    (void)len;
    (void)prot;
    return 0; // ignore until page-prot tracking exists
}

int64_t sys_madvise(SyscallContext& ctx, uint64_t addr, uint64_t len, uint64_t behav) {
    (void)ctx;
    (void)addr;
    (void)len;
    (void)behav;
    return 0;
}

int64_t sys_obs_vhangup(SyscallContext&) { return -ENOSYS; }
int64_t sys_obs_vlimit(SyscallContext&) { return -ENOSYS; }


int64_t sys_mincore(SyscallContext& ctx, uint64_t addr, uint64_t len, uint64_t vec_va) {
    if (!vec_va || !len)
        return -EFAULT;
    // Report all pages resident.
    size_t pages = (static_cast<size_t>(len) + 4095) / 4096;
    std::vector<uint8_t> vec(pages, 1);
    if (!write_guest(ctx, vec_va, vec.data(), pages))
        return -EFAULT;
    (void)addr;
    return 0;
}

int64_t sys_getgroups(SyscallContext& ctx, uint64_t gidsetsize, uint64_t gidset_va) {
#if defined(_WIN32)
    if (gidsetsize == 0)
        return 1;
    if (!gidset_va)
        return -EFAULT;
    uint32_t gid = 0;
    if (!write_guest(ctx, gidset_va, &gid, sizeof(gid)))
        return -EFAULT;
    return 1;
#else
    if (gidsetsize == 0)
        return static_cast<int64_t>(::getgroups(0, nullptr));
    std::vector<gid_t> gids(static_cast<size_t>(gidsetsize));
    int n = ::getgroups(static_cast<int>(gidsetsize), gids.data());
    if (n < 0)
        return -errno;
    if (n > 0 && !write_guest(ctx, gidset_va, gids.data(), sizeof(gid_t) * static_cast<size_t>(n)))
        return -EFAULT;
    return n;
#endif
}

int64_t sys_setgroups(SyscallContext&, uint64_t, uint64_t) {
    return 0;
}

int64_t sys_getpgrp(SyscallContext&) {
#if defined(_WIN32)
    return static_cast<int64_t>(GetCurrentProcessId());
#else
    return static_cast<int64_t>(::getpgrp());
#endif
}

int64_t sys_setpgid(SyscallContext&, uint64_t, uint64_t) {
    return 0;
}

int64_t sys_setitimer(SyscallContext&, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_compat_wait(SyscallContext&, uint64_t) {
    return -ENOSYS;
}

int64_t sys_swapon(SyscallContext&, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_getitimer(SyscallContext&, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_gethostname(SyscallContext& ctx, uint64_t buf_va, uint64_t len) {
    if (!buf_va || !len)
        return -EFAULT;
    char name[256] = "maureps5";
#if defined(_WIN32)
    DWORD n = sizeof(name);
    GetComputerNameA(name, &n);
#else
    if (::gethostname(name, sizeof(name)) != 0)
        std::strncpy(name, "maureps5", sizeof(name) - 1);
#endif
    size_t copy = std::strlen(name);
    if (copy >= static_cast<size_t>(len))
        copy = static_cast<size_t>(len) - 1;
    std::vector<char> out(static_cast<size_t>(len), 0);
    std::memcpy(out.data(), name, copy);
    if (!write_guest(ctx, buf_va, out.data(), static_cast<size_t>(len)))
        return -EFAULT;
    return 0;
}

int64_t sys_sethostname(SyscallContext&, uint64_t, uint64_t) {
    return -EPERM;
}


int64_t sys_getdtablesize(SyscallContext&) {
#if defined(_WIN32)
    return 1024;
#else
    return static_cast<int64_t>(::getdtablesize());
#endif
}

int64_t sys_dup2(SyscallContext&, uint64_t oldfd, uint64_t newfd) {
#if defined(_WIN32)
    if (oldfd == newfd)
        return static_cast<int64_t>(newfd);
    HANDLE h = fd_to_handle(static_cast<int>(oldfd));
    if (h == INVALID_HANDLE_VALUE)
        return -EBADF;
    // Simple: close target slot if used, duplicate into that index if free path
    if (newfd < 3)
        return -EINVAL;
    if (newfd >= 1024)
        return -EBADF;
    if (g_fd_used[static_cast<int>(newfd)]) {
        CloseHandle(g_fds[static_cast<int>(newfd)]);
        free_fd(static_cast<int>(newfd));
    }
    HANDLE proc = GetCurrentProcess();
    HANDLE duped = nullptr;
    if (!DuplicateHandle(proc, h, proc, &duped, 0, FALSE, DUPLICATE_SAME_ACCESS))
        return win_err();
    g_fd_used[static_cast<int>(newfd)] = true;
    g_fds[static_cast<int>(newfd)] = duped;
    return static_cast<int64_t>(newfd);
#else
    int r = ::dup2(static_cast<int>(oldfd), static_cast<int>(newfd));
    if (r < 0)
        return -errno;
    return r;
#endif
}

int64_t sys_fcntl(SyscallContext&, uint64_t fd, uint64_t cmd, uint64_t arg) {
#if defined(_WIN32)
    (void)fd;
    (void)cmd;
    (void)arg;
    return 0;
#else
    int r = ::fcntl(static_cast<int>(fd), static_cast<int>(cmd), static_cast<int>(arg));
    if (r < 0)
        return -errno;
    return r;
#endif
}

int64_t sys_select(SyscallContext&, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_fsync(SyscallContext&, uint64_t fd) {
#if defined(_WIN32)
    HANDLE h = fd_to_handle(static_cast<int>(fd));
    if (h == INVALID_HANDLE_VALUE)
        return -EBADF;
    if (!FlushFileBuffers(h))
        return win_err();
    return 0;
#else
    if (::fsync(static_cast<int>(fd)) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_setpriority(SyscallContext&, uint64_t, uint64_t, uint64_t) {
    return 0;
}

int64_t sys_getpriority(SyscallContext&, uint64_t, uint64_t) {
    return 0;
}

int64_t sys_socket(SyscallContext&, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_connect(SyscallContext&, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_netcontrol(SyscallContext&, uint64_t, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_netabort(SyscallContext&, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_netgetsockinfo(SyscallContext&, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_sigreturn(SyscallContext&, uint64_t) {
    return -ENOSYS;
}


int64_t sys_bind(SyscallContext&, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_setsockopt(SyscallContext&, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_listen(SyscallContext&, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_obs_vtimes(SyscallContext&) {
    return -ENOSYS;
}

int64_t sys_sigvec(SyscallContext&, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_sigblock(SyscallContext&, uint64_t) {
    return -ENOSYS;
}

int64_t sys_sigsetmask(SyscallContext&, uint64_t) {
    return -ENOSYS;
}

int64_t sys_sigsuspend(SyscallContext&, uint64_t) {
    return -ENOSYS;
}

int64_t sys_sigstack(SyscallContext&, uint64_t, uint64_t) {
    return -ENOSYS;
}


int64_t sys_socketex(SyscallContext&, uint64_t, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_socketclose(SyscallContext& ctx, uint64_t fd) {
    return sys_close(ctx, fd);
}

int64_t sys_obs_vtrace(SyscallContext&) {
    return -ENOSYS;
}

int64_t sys_gettimeofday(SyscallContext& ctx, uint64_t tv_va, uint64_t tz_va) {
    if (!tv_va)
        return -EFAULT;
#if defined(_WIN32)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    // Windows epoch -> Unix-ish: 100ns since 1601 -> sec/usec since 1970
    const uint64_t EPOCH_DIFF = 116444736000000000ULL;
    uint64_t t = (uli.QuadPart - EPOCH_DIFF);
    int64_t sec = static_cast<int64_t>(t / 10000000ULL);
    int32_t usec = static_cast<int32_t>((t % 10000000ULL) / 10ULL);
    int64_t tv[2] = {sec, static_cast<int64_t>(usec)};
    if (!write_guest(ctx, tv_va, tv, sizeof(tv)))
        return -EFAULT;
#else
    struct timeval tv{};
    if (::gettimeofday(&tv, nullptr) != 0)
        return -errno;
    if (!write_guest(ctx, tv_va, &tv, sizeof(tv)))
        return -EFAULT;
#endif
    if (tz_va) {
        uint8_t zero[8]{};
        write_guest(ctx, tz_va, zero, sizeof(zero));
    }
    return 0;
}

int64_t sys_getrusage(SyscallContext& ctx, uint64_t who, uint64_t usage_va) {
    (void)who;
    if (!usage_va)
        return -EFAULT;
    uint8_t zero[128]{};
    if (!write_guest(ctx, usage_va, zero, sizeof(zero)))
        return -EFAULT;
    return 0;
}

int64_t sys_getsockopt(SyscallContext&, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_readv(SyscallContext& ctx, uint64_t fd, uint64_t iov_va, uint64_t iovcnt) {
    // Minimal: treat as single read of first iovec if we can parse 16-byte iovec {base,len}
    if (!iov_va || !iovcnt)
        return -EINVAL;
    struct { uint64_t base; uint64_t len; } iov{};
    if (!read_guest(ctx, iov_va, &iov, sizeof(iov)))
        return -EFAULT;
    return sys_read(ctx, fd, iov.base, iov.len);
}

int64_t sys_writev(SyscallContext& ctx, uint64_t fd, uint64_t iov_va, uint64_t iovcnt) {
    if (!iov_va || !iovcnt)
        return -EINVAL;
    int64_t total = 0;
    for (uint64_t i = 0; i < iovcnt && i < 64; ++i) {
        struct { uint64_t base; uint64_t len; } iov{};
        if (!read_guest(ctx, iov_va + i * sizeof(iov), &iov, sizeof(iov)))
            return -EFAULT;
        int64_t r = sys_write(ctx, fd, iov.base, iov.len);
        if (r < 0)
            return r;
        total += r;
        if (static_cast<uint64_t>(r) < iov.len)
            break;
    }
    return total;
}

int64_t sys_settimeofday(SyscallContext&, uint64_t, uint64_t) {
    return -EPERM;
}

int64_t sys_fchown(SyscallContext&, uint64_t, uint64_t, uint64_t) {
    return 0;
}

int64_t sys_fchmod(SyscallContext&, uint64_t, uint64_t) {
    return 0;
}

int64_t sys_netgetiflist(SyscallContext&, uint64_t, uint64_t) {
    return -ENOSYS;
}

int64_t sys_setreuid(SyscallContext&, uint64_t, uint64_t) {
    return 0;
}

int64_t sys_setregid(SyscallContext&, uint64_t, uint64_t) {
    return 0;
}

int64_t sys_rename(SyscallContext& ctx, uint64_t from_va, uint64_t to_va) {
    std::string from = host_path(ctx, read_guest_string(ctx, from_va));
    std::string to = host_path(ctx, read_guest_string(ctx, to_va));
    if (from.empty() || to.empty())
        return -EFAULT;
#if defined(_WIN32)
    if (!MoveFileExA(from.c_str(), to.c_str(), MOVEFILE_REPLACE_EXISTING))
        return win_err();
    return 0;
#else
    if (::rename(from.c_str(), to.c_str()) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_truncate(SyscallContext& ctx, uint64_t path_va, uint64_t length) {
    std::string path = host_path(ctx, read_guest_string(ctx, path_va));
    if (path.empty())
        return -EFAULT;
#if defined(_WIN32)
    HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE)
        return win_err();
    LARGE_INTEGER li;
    li.QuadPart = static_cast<LONGLONG>(length);
    if (!SetFilePointerEx(h, li, nullptr, FILE_BEGIN) || !SetEndOfFile(h)) {
        CloseHandle(h);
        return win_err();
    }
    CloseHandle(h);
    return 0;
#else
    if (::truncate(path.c_str(), static_cast<off_t>(length)) < 0)
        return -errno;
    return 0;
#endif
}

int64_t sys_ftruncate(SyscallContext&, uint64_t fd, uint64_t length) {
#if defined(_WIN32)
    HANDLE h = fd_to_handle(static_cast<int>(fd));
    if (h == INVALID_HANDLE_VALUE)
        return -EBADF;
    LARGE_INTEGER li;
    li.QuadPart = static_cast<LONGLONG>(length);
    if (!SetFilePointerEx(h, li, nullptr, FILE_BEGIN) || !SetEndOfFile(h))
        return win_err();
    return 0;
#else
    if (::ftruncate(static_cast<int>(fd), static_cast<off_t>(length)) < 0)
        return -errno;
    return 0;
#endif
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
    case SYS_sigaltstack:
        return sys_sigaltstack(ctx, a1, a2);
    case SYS_ioctl:
        return sys_ioctl(ctx, a1, a2, a3);
    case SYS_reboot:
        return sys_reboot(ctx, a1);
    case SYS_revoke:
        return sys_revoke(ctx, a1);
    case SYS_symlink:
        return sys_symlink(ctx, a1, a2);
    case SYS_readlink:
        return sys_readlink(ctx, a1, a2, a3);
    case SYS_execve:
        return sys_execve(ctx, a1, a2, a3);
    case SYS_umask:
        return sys_umask(ctx, a1);
    case SYS_chroot:
        return sys_chroot(ctx, a1);
    case SYS_compat_fstat:
        return sys_fstat(ctx, a1, a2);
    case SYS_compat_getkerninfo:
        return sys_getkerninfo(ctx, a1, a2, a3, a4);
    case SYS_compat_getpagesize:
        return sys_getpagesize(ctx);
    case SYS_msync:
        return sys_msync(ctx, a1, a2, a3);
    case SYS_vfork:
        return sys_vfork(ctx);
    case SYS_obs_vread:
        return sys_obs_vread(ctx);
    case SYS_obs_vwrite:
        return sys_obs_vwrite(ctx);
    case SYS_sbrk:
        return sys_sbrk(ctx, a1);
    case SYS_sstk:
        return sys_sstk(ctx, a1);
    case SYS_compat_mmap:
        return sys_mmap(ctx, a1, a2, a3, a4, a5, a6);
    case SYS_obsolete72:
        return enosys();
    case SYS_munmap:
        return sys_munmap(ctx, a1, a2);
    case SYS_mprotect:
        return sys_mprotect(ctx, a1, a2, a3);
    case SYS_madvise:
        return sys_madvise(ctx, a1, a2, a3);
    case SYS_obs_vhangup:
        return sys_obs_vhangup(ctx);
    case SYS_obs_vlimit:
        return sys_obs_vlimit(ctx);
    case SYS_mincore:
        return sys_mincore(ctx, a1, a2, a3);
    case SYS_getgroups:
        return sys_getgroups(ctx, a1, a2);
    case SYS_setgroups:
        return sys_setgroups(ctx, a1, a2);
    case SYS_getpgrp:
        return sys_getpgrp(ctx);
    case SYS_setpgid:
        return sys_setpgid(ctx, a1, a2);
    case SYS_setitimer:
        return sys_setitimer(ctx, a1, a2, a3);
    case SYS_compat_wait:
        return sys_compat_wait(ctx, a1);
    case SYS_swapon:
        return sys_swapon(ctx, a1, a2);
    case SYS_getitimer:
        return sys_getitimer(ctx, a1, a2);
    case SYS_compat_gethostname:
        return sys_gethostname(ctx, a1, a2);
    case SYS_compat_sethostname:
        return sys_sethostname(ctx, a1, a2);
    case SYS_getdtablesize:
        return sys_getdtablesize(ctx);
    case SYS_dup2:
        return sys_dup2(ctx, a1, a2);
    case SYS_number91:
        return enosys();
    case SYS_fcntl:
        return sys_fcntl(ctx, a1, a2, a3);
    case SYS_select:
        return sys_select(ctx, a1, a2, a3, a4, a5);
    case SYS_number94:
        return enosys();
    case SYS_fsync:
        return sys_fsync(ctx, a1);
    case SYS_setpriority:
        return sys_setpriority(ctx, a1, a2, a3);
    case SYS_socket:
        return sys_socket(ctx, a1, a2, a3);
    case SYS_connect:
        return sys_connect(ctx, a1, a2, a3);
    case SYS_netcontrol:
        return sys_netcontrol(ctx, a1, a2, a3, a4);
    case SYS_getpriority:
        return sys_getpriority(ctx, a1, a2);
    case SYS_netabort:
        return sys_netabort(ctx, a1, a2);
    case SYS_netgetsockinfo:
        return sys_netgetsockinfo(ctx, a1, a2);
    case SYS_compat_sigreturn:
        return sys_sigreturn(ctx, a1);
    case SYS_bind:
        return sys_bind(ctx, a1, a2, a3);
    case SYS_setsockopt:
        return sys_setsockopt(ctx, a1, a2, a3, a4, a5);
    case SYS_listen:
        return sys_listen(ctx, a1, a2);
    case SYS_obs_vtimes:
        return sys_obs_vtimes(ctx);
    case SYS_compat_sigvec:
        return sys_sigvec(ctx, a1, a2, a3);
    case SYS_compat_sigblock:
        return sys_sigblock(ctx, a1);
    case SYS_compat_sigsetmask:
        return sys_sigsetmask(ctx, a1);
    case SYS_compat_sigsuspend:
        return sys_sigsuspend(ctx, a1);
    case SYS_compat_sigstack:
        return sys_sigstack(ctx, a1, a2);
    case SYS_socketex:
        return sys_socketex(ctx, a1, a2, a3, a4);
    case SYS_socketclose:
        return sys_socketclose(ctx, a1);
    case SYS_obs_vtrace:
        return sys_obs_vtrace(ctx);
    case SYS_gettimeofday:
        return sys_gettimeofday(ctx, a1, a2);
    case SYS_getrusage:
        return sys_getrusage(ctx, a1, a2);
    case SYS_getsockopt:
        return sys_getsockopt(ctx, a1, a2, a3, a4, a5);
    case SYS_number119:
        return enosys();
    case SYS_readv:
        return sys_readv(ctx, a1, a2, a3);
    case SYS_writev:
        return sys_writev(ctx, a1, a2, a3);
    case SYS_settimeofday:
        return sys_settimeofday(ctx, a1, a2);
    case SYS_fchown:
        return sys_fchown(ctx, a1, a2, a3);
    case SYS_fchmod:
        return sys_fchmod(ctx, a1, a2);
    case SYS_netgetiflist:
        return sys_netgetiflist(ctx, a1, a2);
    case SYS_setreuid:
        return sys_setreuid(ctx, a1, a2);
    case SYS_setregid:
        return sys_setregid(ctx, a1, a2);
    case SYS_rename:
        return sys_rename(ctx, a1, a2);
    case SYS_compat_truncate:
        return sys_truncate(ctx, a1, a2);
    case SYS_compat_ftruncate:
        return sys_ftruncate(ctx, a1, a2);
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
