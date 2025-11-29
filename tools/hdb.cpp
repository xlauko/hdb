#include <assert.h>
#include <signal.h>
#include <sys/types.h>

import hdb;
import std;

constexpr auto split(std::string_view line, char delim = ' ') {
    return line
        | std::views::split(delim)
        | std::ranges::views::transform([](auto&& sr) {
            return std::string_view{sr.begin(), sr.end()};
        });
}

struct options {
    [[=hdb::clap::abbreviate]]
    pid_t pid;

    [[=hdb::clap::positional]]
    std::filesystem::path program;

    [[=hdb::clap::trailing_var_arg]]
    std::vector<std::string_view> args;
};

int main(int argc, char *argv[]) try {
    auto args = hdb::clap::args_t(argv + 1, argv + argc);
    // TODO: auto opts = hdb::clap::parse<options>(args);
} catch (hdb::error &err) {
    std::println(std::cerr, "error: ", err.what());
    return 1;
}
