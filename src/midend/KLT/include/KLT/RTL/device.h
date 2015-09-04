
#ifndef __KLT_RTL_DEVICE_H__
#define __KLT_RTL_DEVICE_H__

#include "KLT/RTL/definition.h"

struct klt_memloc_t;

struct klt_device_t {
  size_t device_id;

  struct klt_device_t * parent;

  size_t num_subdevices;
  struct klt_device_t * subdevices;

  size_t num_memlocs;
  struct klt_memloc_t * memlocs;

  void * user_descriptor;
};

extern size_t klt_devices_count;
extern struct klt_device_t ** klt_devices;

void iklt_increase_alloc_devices();
void iklt_device_increase_alloc_subdevices(struct klt_device_t * device);
void iklt_device_increase_alloc_memlocs(struct klt_device_t * device);

struct klt_device_t * klt_get_device_by_id(size_t id);

struct klt_memloc_t * klt_get_matching_memloc(size_t device_id, enum klt_memory_mode_e mode);

struct klt_device_t * klt_create_subdevice(size_t parent_id);

#endif /* __KLT_RTL_DEVICE_H__ */

