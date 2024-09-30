module;

#include <ctype.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include <readline/readline.h>
#include <readline/history.h>

export module readline;

import std;

namespace rl {

    using owning_line_ptr_t = std::unique_ptr< char, decltype([](char* ptr) { std::free(ptr); }) >;

    void add_to_history(const std::string &line) {
        ::add_history(line.c_str());
    }

    std::string history_entry(int index) {
        auto *entry = ::history_get(index);
        return (entry == nullptr) ? "" : entry->line;
    }

    std::string last_history_entry() {
        return history_entry(::history_length - 1);
    }

    std::optional< std::string > readline(const char *prompt) {
        owning_line_ptr_t line(::readline(prompt));
        if (!line) {
            return std::nullopt;
        }

        return line.get();
    }

    constexpr bool history_empty() {
        return ::history_length == 0;
    }

    export std::optional<std::string> readline_or_recall(const char *prompt) {
        if (auto line = readline(prompt)) {
            if (!line->empty()) {
                add_to_history(*line);
                return line;
            } else if (history_empty()) {
                return last_history_entry();
            }
        }

        return std::nullopt;
    }

} // namespace readline
