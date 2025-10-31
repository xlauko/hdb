module;

#include <experimental/meta>

export module hdb:meta;

import std;

namespace hdb {

  template<auto... vals>
  struct replicator_type {
    template<typename func>
      constexpr void operator>>(func body) const {
        (body.template operator()<vals>(), ...);
      }
  };

  template<auto... vals>
  replicator_type<vals...> replicator = {};
  
  export template<typename range_t>
  consteval auto expand(range_t range) {
    std::vector<std::meta::info> args;
    for (auto r : range)
      args.push_back(reflect_constant(r));
    return substitute(^^replicator, args);
  }

} // namespace hdb
