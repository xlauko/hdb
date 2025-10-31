module;

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cerrno>
#include <cstdlib>
#include <unistd.h>

export module hdb:process;

import std;

import :registers;
import :common;
import :error;
import :enums;
import :pipe;

namespace hdb {

    export struct process
    {
        enum class state { stopped, running, exited, terminated };

        static_assert(enum_to_string(state::stopped) == "stopped");

        struct stop_reason
        {
            stop_reason(i32 wait_status);

            state reason;
            u8 info;
        };

        ~process();

        process()                           = delete;
        process(const process &)            = delete;
        process &operator=(const process &) = delete;

        friend std::unique_ptr< process >
        make_process(i32 pid, bool terminate_on_end, bool debug);

        static std::unique_ptr< process > launch(std::filesystem::path path);
        static std::unique_ptr< process > launch(std::filesystem::path path, bool debug);
        static std::unique_ptr< process > attach(pid_t pid);

        void resume();

        stop_reason wait_on_signal();

        pid_t pid() const { return _pid; }

        state state() const { return _state; }

      private:
        process(pid_t pid, bool terminate_on_end, bool is_attached)
            : _pid(pid), _terminate_on_end(terminate_on_end), _is_attached(is_attached) {}

        void read_all_registers(std::size_t thread_index = 0);

        pid_t _pid             = 0;
        bool _terminate_on_end = true;
        bool _is_attached      = true;
        enum state _state      = state::stopped;
    };

    process::~process() {
        if (pid() == 0) {
            return;
        }

        auto signal_and_wait = [this](i32 signal) {
            kill(pid(), signal);
            i32 status = 0;
            waitpid(pid(), &status, 0);
        };

        if (_is_attached) {
            if (state() == state::running) {
                signal_and_wait(SIGSTOP);
            }

            ptrace(PTRACE_DETACH, pid(), nullptr, nullptr);
            kill(pid(), SIGCONT);
        }

        if (_terminate_on_end) {
            signal_and_wait(SIGKILL);
        }
    }

    std::unique_ptr< process > make_process(i32 pid, bool terminate_on_end, bool debug) {
        return std::unique_ptr< process >(new process(pid, terminate_on_end, debug));
    }

    [[noreturn]] void exit_with_perror(hdb::pipe &channel, const std::string &prefix) {
        auto msg = prefix + ": " + std::strerror(errno);
        channel.write(std::as_bytes(std::span(msg)));
        std::exit(EXIT_FAILURE);
    }

    std::unique_ptr< process > process::launch(std::filesystem::path path) {
        return launch(path, /* debug */ true);
    }

    std::unique_ptr< process > process::launch(std::filesystem::path path, bool debug) {
        pipe channel(/* close on exec */ true);

        pid_t pid;
        if ((pid = fork()) < 0) {
            error::send_errno("fork failed");
        } else if (pid == 0) {
            channel.close_read();
            if (debug && ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) < 0) {
                exit_with_perror(channel, "tracing failed");
            }

            if (execlp(path.c_str(), path.c_str(), nullptr) < 0) {
                exit_with_perror(channel, "exec failed");
            }
        }

        channel.close_write();
        auto data = channel.read();
        channel.close_read();

        if (!data.empty()) {
            waitpid(pid, nullptr, 0);
            error::send(data);
        }

        auto proc = make_process(pid, /* terminate on end */ true, debug);
        if (debug) {
            proc->wait_on_signal();
        }
        return proc;
    }

    std::unique_ptr< process > process::attach(pid_t pid) {
        if (pid == 0) {
            error::send("invalid pid");
        }

        if (ptrace(PTRACE_ATTACH, pid, nullptr, nullptr) < 0) {
            error::send_errno("attach failed");
        }

        auto proc = make_process(pid, /* terminate on end */ false, /* attached */ true);
        proc->wait_on_signal();

        return proc;
    }

    void process::resume() {
        if (ptrace(PTRACE_CONT, pid(), nullptr, nullptr) < 0) {
            error::send_errno("could not resume");
        }

        _state = state::running;
    }

    process::stop_reason::stop_reason(i32 wait_status) {
        if (WIFSTOPPED(wait_status)) {
            reason = state::stopped;
            info   = u8(WSTOPSIG(wait_status));
        } else if (WIFEXITED(wait_status)) {
            reason = state::exited;
            info   = u8(WEXITSTATUS(wait_status));
        } else if (WIFSIGNALED(wait_status)) {
            reason = state::terminated;
            info   = u8(WTERMSIG(wait_status));
        } else {
            error::send("unknown wait status");
        }
    }

    process::stop_reason process::wait_on_signal() {
        i32 status  = 0;
        i32 options = 0;
        if (waitpid(pid(), &status, options) < 0) {
            error::send_errno("waitpid failed");
        }

        stop_reason reason(status);
        _state = reason.reason;

        if (_is_attached && _state == state::stopped) {
            read_all_registers();
        }

        return reason;
    }

    void process::read_all_registers(std::size_t) {}

} // namespace hdb
