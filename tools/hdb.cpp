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


int main() try {
} catch (hdb::error &err) {
    std::println(std::cerr, "error: ", err.what());
    return 1;
}
