
#ifndef __KLT_RTL_DATA_H__
#define __KLT_RTL_DATA_H__

#include "KLT/RTL/definition.h"

struct klt_data_container_t {
  size_t num_param;
  size_t * sizeof_param;

  size_t num_data;
  size_t * sizeof_data;
  size_t * ndims_data;
};

struct klt_data_section_t {
  int offset;
  int length;
//int stride;
};

struct klt_data_t {
  void * ptr;

  size_t base_type_size;

  size_t num_sections;
  struct klt_data_section_t * sections;

  enum klt_memory_mode_e mode;
  enum klt_liveness_e liveness;
};

/// return 0 if the data are the same
int klt_same_data(struct klt_data_t * d1, struct klt_data_t * d2);

#endif /* __KLT_RTL_DATA_H__ */

