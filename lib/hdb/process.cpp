module;

#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <mach/mach_error.h>
#include <mach/task.h>
#include <mach/thread_act.h>

#include <sys/ptrace.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <signal.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>

export module hdb:process;

import std;

import :registers;
import :common;
import :error;
import :pipe;

namespace hdb {

    export struct process {

        enum class state {
            running, stopped, exited, terminated
        };

        struct stop_reason {
            stop_reason(i32 wait_status);

            state reason;
            u8 info;
        };

        ~process();

        process() = delete;
        process(const process &) = delete;
        process &operator=(const process &) = delete;

        friend std::unique_ptr< process > make_process(i32 pid, bool terminate_on_end, bool debug);

        static std::unique_ptr< process > launch(std::filesystem::path path);
        static std::unique_ptr< process > launch(std::filesystem::path path, bool debug);
        static std::unique_ptr< process > attach(pid_t pid);

        void resume();

        stop_reason wait_on_signal();

        pid_t pid() const { return _pid; }
        state state() const { return _state; }

        registers& get_registers() { return *_registers; }
        const registers& get_registers() const { return *_registers; }

      private:

        process(pid_t pid, bool terminate_on_end, bool is_attached)
            : _pid(pid)
            , _terminate_on_end(terminate_on_end)
            , _is_attached(is_attached)
            , _registers(std::make_unique< registers >())
        {}

        void read_all_registers(std::size_t thread_index = 0);
        void sync_registers_to_thread(std::size_t thread_index = 0);

        pid_t _pid = 0;
        bool _terminate_on_end = true;
        bool _is_attached = true;
        enum state _state = state::stopped;

        std::unique_ptr< registers > _registers;
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

            ptrace(PT_DETACH, pid(), {}, {});
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
            if (debug && ptrace(PT_TRACE_ME, 0, {}, {}) < 0) {
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

        auto p = make_process(pid, /* terminate on end */ true, debug);
        if (debug) {
            p->wait_on_signal();
        }
        return p;
    }

    std::unique_ptr< process > process::attach(pid_t pid) {
        if (pid == 0) {
            error::send("invalid pid");
        }

        if (ptrace(PT_ATTACHEXC, pid, {}, {}) < 0) {
            error::send_errno("attach failed");
        }

        auto p = make_process(pid, /* terminate on end */ false, /* attached */ true);
        p->wait_on_signal();
        return p;
    }

    void process::resume() {
        if (ptrace(PT_CONTINUE, pid(), caddr_t(1), {}) < 0) {
            error::send_errno("could not resume");
        }

        _state = state::running;
    }

    process::stop_reason::stop_reason(i32 wait_status) {
        if (WIFSTOPPED(wait_status)) {
            reason = state::stopped;
            info = u8(WSTOPSIG(wait_status));
        } else if (WIFEXITED(wait_status)) {
            reason = state::exited;
            info = u8(WEXITSTATUS(wait_status));
        } else if (WIFSIGNALED(wait_status)) {
            reason = state::terminated;
            info = u8(WTERMSIG(wait_status));
        } else {
            error::send("unknown wait status");
        }
    }

    process::stop_reason process::wait_on_signal() {
        i32 status = 0;
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

    mach_port_t get_task_for_pid(pid_t pid) {
        mach_port_t task;

        if (task_for_pid(mach_task_self(), pid, &task) != KERN_SUCCESS) {
            error::send_errno("Could not get task for process");
        }

        return task;
    }

    thread_t get_thread_for_pid(pid_t pid, std::size_t thread_index) {
        mach_port_t task = get_task_for_pid(pid);
        thread_act_array_t thread_list;
        mach_msg_type_number_t thread_count;

        if (task_threads(task, &thread_list, &thread_count) != KERN_SUCCESS || thread_count == 0) {
            error::send_errno("Could not get thread list");
        }

        thread_t thread = thread_list[thread_index];

        vm_deallocate(
            mach_task_self(),
            std::bit_cast< vm_address_t >(thread_list),
            thread_count * sizeof(thread_t)
        );

        return thread;
    }

    void process::read_all_registers(std::size_t thread_index) {
        thread_t thread = get_thread_for_pid(_pid, thread_index);

        // Read General-Purpose Registers
        mach_msg_type_number_t state_count = ARM_THREAD_STATE64_COUNT;

        if (thread_get_state(
            thread, ARM_THREAD_STATE64,
            std::bit_cast< thread_state_t >(&_registers->_gpr),
            &state_count) != KERN_SUCCESS
        ) {
            error::send_errno("Could not read GPR registers");
        }
    }

    void process::sync_registers_to_thread(std::size_t thread_index) {
        // Assuming we are writing to the first thread for now
        thread_t thread = get_thread_for_pid(_pid, thread_index);

        // Write General-Purpose Registers
        arm_thread_state64_t gpr = _registers->_gpr;
        mach_msg_type_number_t state_count = ARM_THREAD_STATE64_COUNT;

        if (thread_set_state(thread, ARM_THREAD_STATE64, std::bit_cast< thread_state_t >(&gpr), state_count) != KERN_SUCCESS) {
            error::send_errno("Could not write GPR registers");
        }
    }


} // namespace hdb
