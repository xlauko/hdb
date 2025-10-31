#include <doctest/doctest.h>

#include <csignal>

import std;
import hdb;

namespace {

    bool process_exists(pid_t pid) {
        auto ret = kill(pid, 0);
        return ret != -1 && errno != ESRCH;
    }

    char get_process_status(pid_t pid) {
        const auto path = std::format("/proc/{}/stat", pid);
        std::ifstream stat_file(path);
        if (!stat_file) {
            hdb::error::send_errno(std::format("failed to open {}", path));
        }

        std::string data;
        if (!std::getline(stat_file, data)) {
            hdb::error::send_errno(std::format("failed to read from {}", path));
        }

        // Find the closing parenthesis of the process name
        const auto last_paren = data.rfind(')');
        if (last_paren == std::string::npos || last_paren + 2 >= data.size()) {
            hdb::error::send_errno("malformed /proc/[pid]/stat file");
        }

        // Status character is immediately after the space following the last ')'
        return data[last_paren + 2];
    }

} // namespace

namespace hdb::test {

    TEST_SUITE_BEGIN("process");

    TEST_CASE("process::launch success") {
        auto proc = process::launch("yes");
        REQUIRE(process_exists(proc->pid()));
    } // process::launch success

    TEST_CASE("process::launch failure") {
        REQUIRE_THROWS_AS(process::launch("nonexistent"), hdb::error);
    } // process::launch failure


    TEST_CASE("process::attach success") {
        auto proc = process::launch(cfg::test_targets_directory / "endless", false);
        auto attached = process::attach(proc->pid());
        REQUIRE(attached->pid() == proc->pid());
        REQUIRE(get_process_status(proc->pid()) == 't'); 
    } // process::attach success

    TEST_CASE("process::attach failure") {
        REQUIRE_THROWS_AS(process::attach(0), hdb::error);
    } // process::attach failure

    TEST_CASE("process::resume success") {

        SUBCASE("simple") {
            auto proc = process::launch(cfg::test_targets_directory / "endless");
            proc->resume();
            auto status = get_process_status(proc->pid());
            REQUIRE((status == 'R' || status == 'S'));
        }

        SUBCASE("attached") {
            auto proc = process::launch(cfg::test_targets_directory / "endless", false);
            auto attached = process::attach(proc->pid());
            attached->resume();
            auto status = get_process_status(proc->pid());
            REQUIRE((status == 'R' || status == 'S'));
        }
    } // process::attach success

    TEST_CASE("process::resume already terminated") {
        auto proc = process::launch(cfg::test_targets_directory / "terminated");
        proc->resume();
        proc->wait_on_signal();
        proc->resume();
        // REQUIRE_THROWS_AS(proc->resume(), hdb::error);
    } // process::resume already terminated

    TEST_SUITE_END(); // process
} // namespace hdb::test
