module;

#include <fcntl.h>
#include <unistd.h>

export module hdb:pipe;

import std;
import :error;

namespace hdb {

    export struct pipe {
        explicit pipe(bool close_on_exec);

        pipe(const pipe&) = delete;
        pipe& operator=(const pipe&) = delete;

        pipe(pipe&& other) noexcept;
        pipe& operator=(pipe&& other) noexcept;

        ~pipe();

        int get_read() const noexcept  { return _fds[read_fd]; }
        int get_write() const noexcept { return _fds[write_fd]; }

        int realease_read() noexcept  { return release(read_fd); }
        int realease_write() noexcept { return release(write_fd); }

        void close_read() noexcept  { close(_fds[read_fd]); }
        void close_write() noexcept { close(_fds[write_fd]); }

        [[nodiscard]] std::vector< std::byte > read();
        void write(std::span< const std::byte > data);

    private:
        static void close(int &fd) noexcept {
            if (fd != -1) {
                ::close(fd);
                fd = -1;
            }
        }

        int release(unsigned fd) noexcept { return std::exchange(_fds[fd], -1); }

        static constexpr unsigned int read_fd = 0;
        static constexpr unsigned int write_fd = 1;
        int _fds[2] = {-1, -1};
    };

    pipe::pipe(bool close_on_exec) {
        if (::pipe(_fds) < 0) {
            error::send_errno("pipe creation failed");
        }

        if (close_on_exec) {
            for (auto& fd : _fds) {
                if (::fcntl(fd, F_SETFD, FD_CLOEXEC) < 0) {
                    error::send_errno("setting close-on-exec flag failed");
                }
            }
        }
    }

    pipe::pipe(pipe&& other) noexcept {
        for (auto fd : {read_fd, write_fd}) {
            _fds[fd] = other.release(fd);
        }
    }

    pipe& pipe::operator=(pipe&& other) noexcept {
        if (this != &other) {
            for (auto fd : {read_fd, write_fd}) {
                _fds[fd] = other.release(fd);
            }
        }
        return *this;
    }

    pipe::~pipe() {
        for (auto& fd : _fds) { close(fd); }
    }

    std::vector< std::byte > pipe::read() {
        std::vector< std::byte > buffer(1024);
        ssize_t bytes_read = ::read(_fds[read_fd], buffer.data(), buffer.size());
        if (bytes_read < 0) {
            error::send_errno("pipe read failed");
        }

        buffer.resize(static_cast< size_t >(bytes_read));
        return buffer;
    }

    void pipe::write(std::span< const std::byte > data) {
        if (::write(_fds[write_fd], data.data(), data.size()) < 0) {
            error::send_errno("pipe write failed");
        }
    }

} // namespace hdb
