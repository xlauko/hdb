module;

#include <signal.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>

export module hdb:process;

import std;
import :pipe;
import :error;

namespace hdb {

    export struct process {

        enum class state {
            running, stopped, exited, terminated
        };

        struct stop_reason {
            stop_reason(int wait_status);

            state reason;
            int info;
        };

        process() = delete;
        process(const process &) = delete;
        process &operator=(const process &) = delete;

        friend std::unique_ptr< process > make_process(int pid, bool terminate_on_end);

        ~process();

        static std::unique_ptr< process > launch(std::filesystem::path path);
        static std::unique_ptr< process > attach(pid_t pid);

        void resume();

        stop_reason wait_on_signal();

        pid_t pid() const { return _pid; }
        state state() const { return _state; }
      private:

        process(pid_t pid, bool terminate_on_end)
            : _pid(pid), _terminate_on_end(terminate_on_end)
        {}

        pid_t _pid;
        bool _terminate_on_end = true;
        enum state _state = state::stopped;
    };

    process::~process() {
        if (pid() == 0) {
            return;
        }

        auto signal_and_wait = [this](int signal) {
            kill(pid(), signal);
            int status = 0;
            waitpid(pid(), &status, 0);
        };

        if (state() == state::running) {
            signal_and_wait(SIGSTOP);
        }

        ptrace(PT_DETACH, pid(), {}, {});
        kill(pid(), SIGCONT);

        if (_terminate_on_end) {
            signal_and_wait(SIGKILL);
        }
    }

    std::unique_ptr< process > make_process(int pid, bool terminate_on_end) {
        return std::unique_ptr< process >(new process(pid, terminate_on_end));
    }

    void exit_with_perror(hdb::pipe &channel, const std::string &prefix) {
        auto msg = prefix + ": " + std::strerror(errno);
        channel.write(std::as_bytes(std::span(msg)));
        std::exit(EXIT_FAILURE);
    }

    std::unique_ptr< process > process::launch(std::filesystem::path path) {
        pipe channel(/* close on exec */ true);

        pid_t pid;
        if ((pid = fork()) < 0) {
            error::send_errno("fork failed");
        } else if (pid == 0) {
            channel.close_read();
            if (ptrace(PT_TRACE_ME, 0, {}, {}) < 0) {
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

        auto p = make_process(pid, /* terminate on end */ true);
        p->wait_on_signal();
        return p;
    }

    std::unique_ptr< process > process::attach(pid_t pid) {
        if (pid == 0) {
            error::send("invalid pid");
        }

        if (ptrace(PT_ATTACHEXC, pid, {}, {}) < 0) {
            error::send_errno("attach failed");
        }

        auto p = make_process(pid, /* terminate on end */ false);
        p->wait_on_signal();
        return p;
    }

    void process::resume() {
        if (ptrace(PT_CONTINUE, pid(), caddr_t(1), {}) < 0) {
            error::send_errno("could not resume");
        }

        _state = state::running;
    }

    process::stop_reason::stop_reason(int wait_status) {
        if (WIFSTOPPED(wait_status)) {
            reason = state::stopped;
            info = WSTOPSIG(wait_status);
        } else if (WIFEXITED(wait_status)) {
            reason = state::exited;
            info = WEXITSTATUS(wait_status);
        } else if (WIFSIGNALED(wait_status)) {
            reason = state::terminated;
            info = WTERMSIG(wait_status);
        } else {
            error::send("unknown wait status");
        }
    }

    process::stop_reason process::wait_on_signal() {
        int status = 0;
        int options = 0;
        if (waitpid(pid(), &status, options) < 0) {
            error::send_errno("waitpid failed");
        }

        stop_reason reason(status);
        _state = reason.reason;
        return reason;
    }

} // namespace hdb
