
#ifndef __KLT_RTL_DEFINITION_H__
#define __KLT_RTL_DEFINITION_H__

#include <stddef.h>

enum klt_tile_kind_e {
  e_klt_tile_static = 0,
  e_klt_tile_dynamic = 1
};

enum klt_memory_mode_e {
  e_klt_unknown_mode = 0,
  e_klt_read_only,
  e_klt_write_only,
  e_klt_read_write
};

enum klt_liveness_e {
  e_klt_live_unknown = 0,
  e_klt_live_in,
  e_klt_live_out,
  e_klt_live_inout,
  e_klt_live_not
};

#endif /* __KLT_RTL_DEFINITION_H__ */

