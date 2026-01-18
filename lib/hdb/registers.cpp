export module hdb:registers;

import std;

import :bits;
import :common;
import :error;

namespace hdb::reg {

    export enum class reg_id {};

    export enum class reg_kind { gpr, sub_gpb, fpr, dr };

    export enum class reg_format { uint, double_float, long_double, vector };

    export struct info {
      reg_id id;
      std::string_view name;
      i32 dwarf_id;
      std::size_t size;
      std::size_t offset;
      reg_kind kind;
      reg_format format;
    };

    // inline constexpr const info infos[] = {

    // };
} // namespace hdb::reg
