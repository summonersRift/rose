
#include "KLT/RTL/init.h"

#include "KLT/RTL/memory.h"
#include "KLT/RTL/device.h"

#include <stdlib.h>

#include <assert.h>

void klt_init() {
  iklt_increase_alloc_devices();
  iklt_increase_alloc_memlocs();

  klt_memlocs_count = 1;
  klt_devices[0] = malloc(sizeof(struct klt_device_t));
    klt_devices[0]->device_id = 0;
    klt_devices[0]->parent = NULL;
    klt_devices[0]->num_subdevices = 0;
    klt_devices[0]->subdevices = NULL;
    klt_devices[0]->num_memlocs = 0;
    klt_devices[0]->memlocs = NULL;
    klt_devices[0]->user_descriptor = NULL;

  iklt_device_increase_alloc_memlocs(klt_devices[0]);
  assert(klt_devices[0]->num_memlocs == 1);

  klt_devices_count = 1;
  klt_memlocs[0] = &(klt_devices[0]->memlocs[0]);

    klt_memlocs[0]->memloc_id = 0;
    klt_memlocs[0]->device = klt_devices[0];
    klt_memlocs[0]->mode = e_klt_read_write;
    klt_memlocs[0]->size = 0;
    klt_memlocs[0]->user_descriptor = NULL;
}

