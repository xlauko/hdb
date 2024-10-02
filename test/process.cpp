#include <doctest/doctest.h>

import hdb;

namespace {

    bool process_exists(pid_t pid) {
        auto ret = kill(pid, 0);
        return ret != -1 && errno != ESRCH;
    }

} // namespace

namespace hdb::test {

    TEST_SUITE_BEGIN("process");

    TEST_CASE("process::launch success") {
        auto proc = process::launch("yes");
        REQUIRE(process_exists(proc->pid()));
    } // process::launch success

    TEST_CASE("process::launch failure") {
        // REQUIRE_THROWS_AS(process::launch("nonexistent"), hdb::error);
    } // process::launch failure

    TEST_SUITE_END(); // process
} // namespace hdb::test
