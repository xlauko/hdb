module;

#include <experimental/meta>

export module hdb:enums;

import std;

import :meta;
    
export namespace hdb {

    template< typename type_t >
    concept enum_type = std::is_enum_v< type_t >;

    template< enum_type enum_t >
    constexpr std::string_view enum_to_string(enum_t value) {
        std::string_view result = "<unnamed>";
        [:expand(enumerators_of(^^enum_t)):] >> [&]<auto e>{
            if (value == [:e:])
                result = identifier_of(e);
        };
        return result;
    }

} // namespace hdb
