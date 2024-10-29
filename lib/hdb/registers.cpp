module;

#include <mach/mach.h>
#include <mach/thread_status.h>

#include <cassert>

export module hdb:registers;

import std;

import :bits;
import :common;
import :error;

namespace hdb {

    // register information
    namespace reg {

        export enum class id {
            #define DEFINE_REGISTER(name, ...) name
            #include "registers.inc"
            #undef DEFINE_REGISTER
        };

        export enum class type { gpr, sub_gpr, fpr, dr };

        export enum class format { uint, double_float, long_double, vector };

        export struct info {
            id id;
            std::string_view name;
            i32 dwarf_id;
            std::size_t size;
            type type;
            format format;
        };

    } // namespace reg

    inline constexpr reg::info register_infos[] = {
        #define DEFINE_REGISTER(name, dwarf_id, size, reg_type, reg_format) { \
              reg::id::name, #name, dwarf_id, size \
            , reg::type::reg_type \
            , reg::format::reg_format \
        }
        #include "registers.inc"
        #undef DEFINE_REGISTER
    };

    template< typename func_t >
    const reg::info &register_info_by(func_t &&func) {
        auto it = std::ranges::find_if(register_infos, std::forward< func_t >(func));
        if (it != std::end(register_infos)) {
            return *it;
        }

        error::send("register not found");
    }


    const reg::info &register_info_by_id(reg::id id) {
        return register_info_by([id](const auto &i) { return i.id == id; });
    }

    const reg::info &register_info_by_name(std::string_view name) {
        return register_info_by([name](const auto &i) { return i.name == name; });
    }

    const reg::info &register_info_by_dwarf(i32 dwarf_id) {
        return register_info_by([dwarf_id](const auto &i) { return i.dwarf_id == dwarf_id; });
    }

    export struct registers {

        registers() = default;
        registers(const registers &) = delete;
        registers &operator=(const registers &) = delete;

        using value = std::variant<
            u32, u64, i32, i64,
            f32, f64, f128,
            b64, b128
        >;

        value read(const reg::info &info) const;
        void write(const reg::info &info, value val);

        template< std::copyable result_t >
        result_t read_by_id_as(reg::id id) const;
        void write_by_id(reg::id id, value val);

      private:
        friend struct process;

        arm_thread_state64_t _gpr;
    };

    auto registers::read(const reg::info &info) const -> value {
        switch (info.format) {
            case reg::format::uint:
                assert(info.type == reg::type::gpr);
                switch (info.size) {
                    assert(info.dwarf_id >= 0 && info.dwarf_id < 29);
                    case sizeof(u32): return static_cast< u32 >(_gpr.__x[info.dwarf_id]);
                    case sizeof(u64): return static_cast< u64 >(_gpr.__x[info.dwarf_id]);
                    default: error::send("invalid register size");
                }
            case reg::format::double_float:
                error::send("not implemented");
            case reg::format::long_double:
                error::send("not implemented");
            case reg::format::vector:
                error::send("not implemented");
        }
        return {};
    }

    void registers::write(const reg::info &info, value val) {
        auto bs = bytes(val);

        auto write_gpr = [&] {
            assert(info.type == reg::type::gpr);
            assert(info.dwarf_id >= 0 && info.dwarf_id < 29);
            auto &reg = _gpr.__x[info.dwarf_id];

            switch (info.size) {
                case sizeof(u32): reg = from_bytes< u32 >(bs);
                case sizeof(u64): reg = from_bytes< u64 >(bs);
            }
        };

        val.visit([&](auto v) {
            if (sizeof(v) == info.size) {
                switch (info.format) {
                    case reg::format::uint: write_gpr(); break;
                    case reg::format::double_float:
                        error::send("not implemented");
                    case reg::format::long_double:
                        error::send("not implemented");
                    case reg::format::vector:
                        error::send("not implemented");
                }
            } else {
                error::send("write called with mismatched register and value sizes");
            }
        });
    }

    template< std::copyable result_t >
    result_t registers::read_by_id_as(reg::id id) const {
        return std::get< result_t >(read(register_info_by_id(id)));
    }

    void registers::write_by_id(reg::id id, value val) {
        write(register_info_by_id(id), val);
    }

} // namespace hdb
