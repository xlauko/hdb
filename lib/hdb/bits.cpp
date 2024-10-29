export module hdb:bits;

import std;

import :common;

export namespace hdb {

    template< typename to_t >
    requires std::is_trivially_copyable_v< to_t >
    constexpr to_t from_bytes(std::span< const std::byte > bs) noexcept {
        to_t result;
        std::memcpy(&result, bs.data(), sizeof(result));
        return result;
    }

    template< typename from_t >
    requires std::is_trivially_copyable_v< from_t >
    constexpr std::span< const std::byte > bytes(const from_t &from) noexcept {
        return { reinterpret_cast< const std::byte * >(&from), sizeof(from) };
    }

    template< typename from_t >
    requires std::is_trivially_copyable_v< from_t >
    constexpr std::span< std::byte > bytes(from_t &from) noexcept {
        return { reinterpret_cast< std::byte * >(&from), sizeof(from) };
    }

    template< typename from_t >
    requires std::is_trivially_copyable_v< from_t > && (sizeof(from_t) == 8)
    constexpr b128 to_b128(const from_t &from) noexcept {
        b128 result;
        std::memcpy(result.data(), &from, sizeof(from));
        return result;
    }

    template< typename from_t >
    requires std::is_trivially_copyable_v< from_t > && (sizeof(from_t) == 4)
    constexpr b64 to_b64(const from_t &from) noexcept {
        b64 result;
        std::memcpy(result.data(), &from, sizeof(from));
        return result;
    }

    constexpr std::string_view to_string_view(std::span< const std::byte > bytes) noexcept {
        return { reinterpret_cast< const char * >(bytes.data()), bytes.size() };
    }

} // namespace hdb
