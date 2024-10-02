module;

#include <sys/errno.h>

export module hdb:error;

import std;

namespace hdb {

    export struct error : std::runtime_error {
        static void send(const std::string &what) {
            throw error(what);
        }

        static void send(std::span< const std::byte > bytes) {
            auto chars = std::views::transform(bytes, [](auto b) {
                return static_cast< const char >(b);
            });
            send(std::string(chars.begin(), chars.end()));
        }

        static void send_errno(const std::string &prefix) {
            send(prefix + ": " + std::strerror(errno));
        }

      private:

        error(const std::string &what)
            : std::runtime_error(what)
        {}
    };
} // namespace hdb
