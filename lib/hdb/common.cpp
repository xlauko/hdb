export module hdb:common;

import std;

export namespace hdb {

    using u8  = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using i8  = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using f32 = float;
    using f64 = double;
    using f128 = long double;

    using b64  = std::array< std::byte, 8 >;
    using b128 = std::array< std::byte, 16 >;

} // namespace hdb
