
#ifndef __KLT_RTL_TILE_H__
#define __KLT_RTL_TILE_H__

#include "KLT/RTL/definition.h"

struct klt_tile_desc_t {
  int idx;
  enum klt_tile_kind_e kind;
  int param;
};

struct klt_tile_t {
  int length;
  int stride;
//int remain;
};

#endif /* __KLT_RTL_TILE_H__ */

