#include <assert.h>
#include <signal.h>
#include <sys/types.h>

import hdb;
import std;
import readline;



auto split(std::string_view line, char delim = ' ') {
    return line
        | std::views::split(delim)
        | std::ranges::views::transform([](auto&& sr) {
            return std::string_view{sr.begin(), sr.end()};
        });
}

std::unique_ptr< hdb::process > attach(int argc, const char* argv[]) {
    if (argc == 3 && argv[1] == std::string_view("-p")) {
        pid_t pid = std::stoi(argv[2]);
        return hdb::process::attach(pid);
    } else {
        std::string_view path = argv[1];
        return hdb::process::launch(path);
    }
}

std::string_view sigabbrev(int sig) {
    switch (sig) {
        case SIGABRT: return "ABRT";
        case SIGALRM: return "ALRM";
        case SIGBUS: return "BUS";
        case SIGCHLD: return "CHLD";
        case SIGCONT: return "CONT";
        case SIGFPE: return "FPE";
        case SIGHUP: return "HUP";
        case SIGILL: return "ILL";
        case SIGINT: return "INT";
        case SIGKILL: return "KILL";
        case SIGPIPE: return "PIPE";
        case SIGQUIT: return "QUIT";
        case SIGSEGV: return "SEGV";
        case SIGSTOP: return "STOP";
        case SIGTERM: return "TERM";
        case SIGTSTP: return "TSTP";
        case SIGTTIN: return "TTIN";
        case SIGTTOU: return "TTOU";
        case SIGUSR1: return "USR1";
        case SIGUSR2: return "USR2";
        case SIGPROF: return "PROF";
        case SIGSYS: return "SYS";
        case SIGTRAP: return "TRAP";
        case SIGURG: return "URG";
        case SIGVTALRM: return "VTALRM";
        case SIGXCPU: return "XCPU";
        case SIGXFSZ: return "XFSZ";
        default: return "UNKNOWN";
    }
}

void print_stop_reason(const hdb::process &proc, const hdb::process::stop_reason &reason) {
    std::print("Process {} ", proc.pid());

    switch (reason.reason) {
        case hdb::process::state::stopped:
            std::println("stopped with signal ", sigabbrev(reason.info));
            break;
        case hdb::process::state::exited:
            std::println("exited with status ", reason.info);
            break;
        case hdb::process::state::terminated:
            std::println("terminated with signal ", sigabbrev(reason.info));
            break;
        default:
            std::unreachable();
    }
}


void handle_command(std::unique_ptr< hdb::process > &proc, std::string_view line) {
    auto args = split(line);

    if (args.empty()) {
        std::println(std::cerr, "No command given");
        return;
    }

    const auto& cmd = args.front();

    if (cmd.starts_with("continue")) {
        proc->resume();
        auto reason = proc->wait_on_signal();
        print_stop_reason(*proc, reason);
    } else {
        std::println(std::cerr, "Unknown command: ", cmd);
    }
}

void main_loop(std::unique_ptr< hdb::process > proc) {
    while (auto line = rl::readline_or_recall("hdb> ")) {
        assert(!line->empty());

        try {
            handle_command(proc, *line);
        } catch (hdb::error &err) {
            std::println(std::cerr, "error: ", err.what());
        }
    }
}

int main(int argc, const char* argv[]) try {
    if (argc == 1) {
        std::println(std::cerr, "Usage: hdb [-p PID] <path>");
        return 1;
    }

    main_loop(attach(argc, argv));
} catch (hdb::error &err) {
    std::println(std::cerr, "error: ", err.what());
    return 1;
}
