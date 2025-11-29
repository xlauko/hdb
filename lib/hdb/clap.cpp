module;

#include <meta>

export module hdb:clap;

import std;

using namespace std::meta;

export namespace hdb::clap {
    inline constexpr struct{} abbreviate [[maybe_unused]] = {};
    inline constexpr struct{} positional [[maybe_unused]] = {};
    inline constexpr struct{} trailing_var_arg [[maybe_unused]] = {};

    using args_t = std::vector<std::string_view>;
} // namespace hdb
