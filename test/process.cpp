#include <doctest/doctest.h>
#include <sys/proc_info.h>
#include <sys/proc.h>
#include <libproc.h>

import std;
import hdb;

namespace {

    bool process_exists(pid_t pid) {
        auto ret = kill(pid, 0);
        return ret != -1 && errno != ESRCH;
    }

    using proc_status = decltype(proc_bsdinfo::pbi_status);

    std::optional< proc_status > get_process_status(pid_t pid) {
        struct proc_bsdinfo bsinfo;
        if (proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &bsinfo, PROC_PIDTBSDINFO_SIZE) != PROC_PIDTBSDINFO_SIZE) {
            return std::nullopt;
        }
        return bsinfo.pbi_status;
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

    auto check_process_status = [] (const auto& proc) {
        auto status = get_process_status(proc->pid());
        REQUIRE(status.has_value());
        bool proc_status = (status.value() == SRUN || status.value() == SSLEEP);
        REQUIRE(proc_status);
    };

    TEST_CASE("process::attach success") {
        auto proc = process::launch(cfg::test_targets_directory / "endless", false);
        auto attached = process::attach(proc->pid());
        REQUIRE(attached->pid() == proc->pid());
        check_process_status(attached);
    } // process::attach success

    TEST_CASE("process::attach failure") {
        REQUIRE_THROWS_AS(process::attach(0), hdb::error);
    } // process::attach failure

    TEST_CASE("process::resume success") {

        SUBCASE("simple") {
            auto proc = process::launch(cfg::test_targets_directory / "endless");
            proc->resume();
            check_process_status(proc);
        }

        SUBCASE("attached") {
            auto proc = process::launch(cfg::test_targets_directory / "endless", false);
            auto attached = process::attach(proc->pid());
            attached->resume();
            check_process_status(attached);
        }
    } // process::attach success

    TEST_CASE("process::resume already terminated") {
        auto proc = process::launch(cfg::test_targets_directory / "terminated");
        proc->resume();
        proc->wait_on_signal();
        REQUIRE_THROWS_AS(proc->resume(), hdb::error);
    } // process::resume already terminated

    TEST_SUITE_END(); // process
} // namespace hdb::test
