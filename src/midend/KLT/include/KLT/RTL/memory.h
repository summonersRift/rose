
#ifndef __KLT_RTL_MEMORY_H__
#define __KLT_RTL_MEMORY_H__

#include "KLT/RTL/definition.h"

struct klt_data_t;
struct klt_device_t;
struct klt_allocation_t;

struct klt_memloc_t {
  size_t memloc_id;

  struct klt_device_t * device;

  enum klt_memory_mode_e mode;

  size_t size; /// for user (set to 0)

  void * user_descriptor;
};

extern size_t klt_memlocs_count;
extern struct klt_memloc_t ** klt_memlocs;

void iklt_increase_alloc_memlocs();

struct klt_memloc_t * klt_get_memloc_by_id(size_t id);

struct klt_memloc_t * klt_create_memloc(size_t device_id, enum klt_memory_mode_e mode, size_t size);

struct klt_allocation_t {
  enum klt_memory_mode_e mode;

  size_t size; /// for user (set to 0)

  void * user_descriptor;
};

/// User-defined
extern struct klt_allocation_t * klt_user_allocate_data_on_device(struct klt_data_t * data, struct klt_memloc_t * memloc);
extern void klt_user_free_data_on_device(struct klt_data_t * data, struct klt_memloc_t * memloc, struct klt_allocation_t * alloc);

#endif /* __KLT_RTL_MEMORY_H__ */

