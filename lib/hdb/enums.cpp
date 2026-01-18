module;

#include <string_view>
#include <type_traits>
#include <meta>

export module hdb:enums;
    
export namespace hdb {

    template< typename type_t >
    concept enum_type = std::is_enum_v< type_t >;

    template< enum_type enum_t >
    constexpr std::string_view enum_to_string(enum_t value) {
        template for (constexpr auto e : std::meta::enumerators_of(^^enum_t)) {
            if (value == [:e:]) {
              return std::meta::identifier_of(e);
            }
        }
        
        return "<unnamed>";
    }

} // namespace hdb
