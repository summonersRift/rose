
#include "sage3basic.h"

#include "DLX/Core/constructs.hpp"
#include "DLX/Core/frontend.hpp"
#include "DLX/Core/directives.hpp"
#include "DLX/TileK/language.hpp"

#include <cassert>

class SgLocatedNode;

namespace DLX {

namespace Directives {

template <>
generic_construct_t<TileK::language_t> * buildConstruct<TileK::language_t>(TileK::language_t::construct_kinds_e kind) {
  switch (kind) {
    case TileK::language_t::e_construct_dataenv:
     return new construct_t<TileK::language_t, TileK::language_t::e_construct_dataenv>();
    case TileK::language_t::e_construct_kernel:
     return new construct_t<TileK::language_t, TileK::language_t::e_construct_kernel>();
    case TileK::language_t::e_construct_loop:
     return new construct_t<TileK::language_t, TileK::language_t::e_construct_loop>();
    case TileK::language_t::e_construct_last:
     assert(false);
    default:
     assert(false);
  }
}

}

template <>
bool Frontend<TileK::language_t>::findAssociatedNodes_tpl(
  SgLocatedNode * directive_node,
  Directives::generic_construct_t<TileK::language_t> * construct
) {
  switch (construct->kind) {
    case TileK::language_t::e_construct_dataenv:
     return findAssociatedNodes<TileK::language_t::e_construct_dataenv>(
       directive_node, (Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_dataenv> *)construct
     );
    case TileK::language_t::e_construct_kernel:
     return findAssociatedNodes<TileK::language_t::e_construct_kernel>(
       directive_node, (Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_kernel> *)construct
     );
    case TileK::language_t::e_construct_loop:
     return findAssociatedNodes<TileK::language_t::e_construct_loop>(
       directive_node, (Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_loop> *)construct
     );
    case TileK::language_t::e_construct_last:
     assert(false);
    default:
     assert(false);
  }
}

}

