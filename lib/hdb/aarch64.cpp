export module hdb:aarch64;

import std;

import :common;


namespace hdb {

    export struct registers {
        using value = std::variant<
            u8, u16, u32, u64,
            i8, i16, i32, i64,
            f32, f64, f128,
            b64, b128
        >;
    };

} // namespace hdb
