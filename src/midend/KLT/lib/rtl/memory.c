
#include "KLT/RTL/memory.h"
#include "KLT/RTL/device.h"

#include <stdlib.h>

#include <assert.h>

//struct klt_memloc_t klt_host_memloc = { 0, &host_device, klt_read_write, 0, NULL };

size_t klt_memlocs_count = 0;
struct klt_memloc_t ** klt_memlocs = NULL;

void iklt_increase_alloc_memlocs() {
  klt_memlocs_count += 1;
  klt_memlocs = realloc(klt_memlocs, klt_memlocs_count * sizeof(struct klt_memloc_t *));
  assert(klt_memlocs != NULL);
}

struct klt_memloc_t * klt_get_memloc_by_id(size_t id) {
  assert(id < klt_memlocs_count);
  return klt_memlocs[id];
}

struct klt_memloc_t * klt_create_memloc(size_t device_id, enum klt_memory_mode_e mode, size_t size) {
  iklt_increase_alloc_memlocs();

  struct klt_device_t * device = klt_get_device_by_id(device_id);
  iklt_device_increase_alloc_memlocs(device);

  struct klt_memloc_t * memloc = &(device->memlocs[device->num_memlocs - 1]);
    memloc->memloc_id = klt_memlocs_count - 1;
    memloc->device = device;
    memloc->mode = mode;
    memloc->size = size;
    memloc->user_descriptor = NULL;

  return memloc;
}

