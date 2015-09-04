
#include "KLT/RTL/device.h"
#include "KLT/RTL/memory.h"
#include "KLT/RTL/data.h"

#include <stdlib.h>

#include <assert.h>

//struct klt_device_t klt_host_device = { 0, NULL, 0, NULL, 1, &klt_host_memloc, NULL };

size_t klt_devices_count = 0;
struct klt_device_t ** klt_devices = NULL;

void iklt_increase_alloc_devices() {
  klt_devices_count += 1;
  klt_devices = realloc(klt_devices, klt_devices_count * sizeof(struct klt_device_t *));
  assert(klt_devices != NULL);
}

void iklt_device_increase_alloc_subdevices(struct klt_device_t * device) {
  device->num_subdevices++;
  device->subdevices = realloc(device->subdevices, device->num_subdevices * sizeof(struct klt_device_t));
  assert(device->subdevices != NULL);
}

void iklt_device_increase_alloc_memlocs(struct klt_device_t * device) {
  device->num_memlocs++;
  device->memlocs = realloc(device->memlocs, device->num_memlocs * sizeof(struct klt_memloc_t));
  assert(device->memlocs != NULL);
}

struct klt_memloc_t * klt_get_matching_memloc(size_t device_id, enum klt_memory_mode_e mode) {
  assert(mode != e_klt_unknown_mode);
  assert(klt_devices != NULL && device_id < klt_devices_count);

  struct klt_device_t * device = klt_devices[device_id];

  size_t i;
  for (i = 0; i < device->num_memlocs; i++)
    if (device->memlocs[i].mode == mode)
      return &(device->memlocs[i]);
  return NULL;
}

struct klt_device_t * klt_get_device_by_id(size_t id) {
  assert(id < klt_devices_count);
  return klt_devices[id];
}

struct klt_device_t * klt_create_subdevice(size_t parent_id) {
  iklt_increase_alloc_devices();

  struct klt_device_t * parent = klt_get_device_by_id(parent_id);
  iklt_device_increase_alloc_subdevices(parent);
  
  struct klt_device_t * device = &(parent->subdevices[parent->num_subdevices - 1]);
    device->device_id = klt_devices_count - 1;
    device->parent = parent;
    device->num_subdevices = 0;
    device->subdevices = NULL;
    device->num_memlocs = 0;
    device->memlocs = NULL;
    device->user_descriptor = NULL;

  return device;
}

