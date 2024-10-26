module;

#include <sys/errno.h>

export module hdb:error;

import std;

import :bits;

namespace hdb {

    export struct error : std::runtime_error {
        [[noreturn]] static void send(const std::string &what) {
            throw error(what);
        }

        [[noreturn]] static void send(std::span< const std::byte > bytes) {
            send(std::string(to_string_view(bytes)));
        }

        [[noreturn]] static void send_errno(const std::string &prefix) {
            send(prefix + ": " + std::strerror(errno));
        }

      private:

        error(const std::string &what)
            : std::runtime_error(what)
        {}
    };
} // namespace hdb
