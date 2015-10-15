
#include "KLT/RTL/init.h"

#include "KLT/RTL/memory.h"
#include "KLT/RTL/device.h"

#if KLT_OPENCL_ENABLED
#  include "KLT/RTL/opencl-utils.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>

void klt_host_init(void);
void klt_host_init(void) {
  size_t device_id = iklt_increase_alloc_devices();
  assert(device_id == 0);

  klt_devices[device_id] = malloc(sizeof(struct klt_device_t));
    klt_devices[device_id]->device_id = device_id;
    klt_devices[device_id]->kind = e_klt_host;
    klt_devices[device_id]->parent = NULL;
    klt_devices[device_id]->num_subdevices = 0;
    klt_devices[device_id]->subdevices = NULL;
    klt_devices[device_id]->num_memlocs = 0;
    klt_devices[device_id]->memlocs = NULL;
    klt_devices[device_id]->descriptor.host = NULL; // Unused

  size_t memloc_idx = iklt_device_increase_alloc_memlocs(klt_devices[device_id]);
  assert(memloc_idx == 0);
  size_t memloc_id = iklt_increase_alloc_memlocs();
  assert(memloc_id == 0);

  klt_memlocs[memloc_id] = &(klt_devices[device_id]->memlocs[memloc_idx]);
    klt_memlocs[memloc_id]->memloc_id = memloc_id;
    klt_memlocs[memloc_id]->device = klt_devices[device_id];
    klt_memlocs[memloc_id]->mode = e_klt_read_write;
    klt_memlocs[memloc_id]->size = 0;
    klt_memlocs[memloc_id]->descriptor = NULL;
}

#if KLT_THREADS_ENABLED
void klt_threads_init(void);
void klt_threads_init(void) {
  size_t num_threads = 8; // TODO get number from environment or system-call
  struct klt_threads_device_t * threads_device = iklt_build_threads_device(num_threads);

  size_t device_idx, device_id, memloc_idx, memloc_id; 

  device_idx = iklt_device_increase_alloc_subdevices(klt_devices[0]);
  device_id = iklt_increase_alloc_devices();
  assert(device_id > 0);

  klt_devices[device_id] = &(klt_devices[0]->subdevices[device_idx]);
    klt_devices[device_id]->device_id = device_id;
    klt_devices[device_id]->kind = e_klt_threads;
    klt_devices[device_id]->parent = klt_devices[0];
    klt_devices[device_id]->num_subdevices = 0;
    klt_devices[device_id]->subdevices = NULL;
    klt_devices[device_id]->num_memlocs = 0;
    klt_devices[device_id]->memlocs = NULL;
    klt_devices[device_id]->descriptor.threads = threads_device;

  memloc_idx = iklt_device_increase_alloc_memlocs(klt_devices[device_id]);
  assert(memloc_idx == 0);
  memloc_id = iklt_increase_alloc_memlocs();
  assert(memloc_id > 0);

  klt_memlocs[memloc_id] = &(klt_devices[device_id]->memlocs[memloc_idx]);
    klt_memlocs[memloc_id]->memloc_id = memloc_id;
    klt_memlocs[memloc_id]->device = klt_devices[device_id];
    klt_memlocs[memloc_id]->mode = e_klt_read_write;
    klt_memlocs[memloc_id]->size = 0;
    klt_memlocs[memloc_id]->descriptor = NULL;
}
#endif /* KLT_THREADS_ENABLED */

#if KLT_OPENCL_ENABLED
extern char * opencl_kernel_file;
extern char * opencl_kernel_options;
extern char * opencl_klt_runtime_lib;

void klt_opencl_init(void);
void klt_opencl_init(void) {
  size_t i, j;
  cl_int status;

  // Sources and Options

  const char * sources[2] = { klt_read_file(opencl_kernel_file) , klt_read_file(opencl_klt_runtime_lib) };

  size_t opts_length = strlen(opencl_kernel_options) + 1;

  char * context_storage_modifier = " -DCOMPILE_FOR_KERNEL=1 -DSTORAGE_MODIFIER=__constant -DDEVICE_FUNCTION_MODIFIER=";
  opts_length += strlen(context_storage_modifier);

#if COMPILE_OPENCL_KERNEL_WITH_DEBUG == 1
  char * debug_flags = " -g";
  opts_length += strlen(debug_flags);
#endif

  char * options = (char *)malloc(opts_length * sizeof(char));
  memset(options, 0, opts_length * sizeof(char));

  strcat(options, opencl_kernel_options);
  strcat(options, context_storage_modifier);
#if COMPILE_OPENCL_KERNEL_WITH_DEBUG == 1
  strcat(options, debug_flags);
#endif

  // Iterate over OpenCL's Platforms & Devices

  cl_uint num_platforms;
  status = clGetPlatformIDs(0, NULL, &num_platforms);
  if (status != CL_SUCCESS) {
    printf("[error]   clGetPlatformIDs return %u when looking for the number of platforms.\n", status);
    return;
  }

  cl_platform_id * platforms = (cl_platform_id *)malloc(num_platforms * sizeof(cl_platform_id));
  assert(platforms != NULL);

  status = clGetPlatformIDs(num_platforms, platforms, NULL);
  if (status != CL_SUCCESS) {
    printf("[error]   clGetPlatformIDs return %u when retrieving %u platforms.\n", status, num_platforms);
    return;
  }

  for (i = 0; i < num_platforms; i++) {
    cl_uint num_devices = 0;

    status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
    if(status != CL_SUCCESS) {
     printf("[warning] clGetDeviceIDs return %u when looking for number of devices for platform %zu\n", status, i);
     continue;
    }

    cl_device_id * devices = (cl_device_id *)malloc(num_devices * sizeof(cl_device_id));
    assert(devices != NULL);

    status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
    if(status != CL_SUCCESS) {
     printf("[warning] clGetDeviceIDs return %u when retrieving %u devices for platform %zu\n", status, num_devices, i);
     free(devices);
     continue;
    }

    for (j = 0; j < num_devices; j++) {
      size_t device_idx, device_id, memloc_idx, memloc_id;

      /// platforms[i] / devices[j]
      struct klt_opencl_device_t * opencl_device = klt_build_opencl_device(platforms[i], devices[j], 2, sources, options);

      device_idx = iklt_device_increase_alloc_subdevices(klt_devices[0]);
      device_id = iklt_increase_alloc_devices();
      assert(device_id > 0);

      klt_devices[device_id] = &(klt_devices[0]->subdevices[device_idx]);
        klt_devices[device_id]->device_id = device_id;
        klt_devices[device_id]->kind = e_klt_opencl;
        klt_devices[device_id]->parent = klt_devices[0];
        klt_devices[device_id]->num_subdevices = 0;
        klt_devices[device_id]->subdevices = NULL;
        klt_devices[device_id]->num_memlocs = 0;
        klt_devices[device_id]->memlocs = NULL;
        klt_devices[device_id]->descriptor.opencl = opencl_device;

      memloc_idx = iklt_device_increase_alloc_memlocs(klt_devices[device_id]);
      assert(memloc_idx == 0);
      memloc_id = iklt_increase_alloc_memlocs();
      assert(memloc_id > 0);

      klt_memlocs[memloc_id] = &(klt_devices[device_id]->memlocs[memloc_idx]);
        klt_memlocs[memloc_id]->memloc_id = memloc_id;
        klt_memlocs[memloc_id]->device = klt_devices[device_id];
        klt_memlocs[memloc_id]->mode = e_klt_read_write;
        klt_memlocs[memloc_id]->size = 0;
        klt_memlocs[memloc_id]->descriptor = NULL;

      memloc_idx = iklt_device_increase_alloc_memlocs(klt_devices[device_id]);
      assert(memloc_idx == 1);
      memloc_id = iklt_increase_alloc_memlocs();
      assert(memloc_id > 0);

      klt_memlocs[memloc_id] = &(klt_devices[device_id]->memlocs[memloc_idx]);
        klt_memlocs[memloc_id]->memloc_id = memloc_id;
        klt_memlocs[memloc_id]->device = klt_devices[device_id];
        klt_memlocs[memloc_id]->mode = e_klt_read_only;
        klt_memlocs[memloc_id]->size = 0;
        klt_memlocs[memloc_id]->descriptor = NULL;

      assert(klt_devices[device_id]->num_memlocs == 2);
    }
    free(devices);
  }
  free(platforms);
  free(options);
  free((void*)sources[0]);
  free((void*)sources[1]);
}
#endif /* KLT_OPENCL_ENABLED */

#if KLT_CUDA_ENABLED
void klt_cuda_init(void);
void klt_cuda_init(void) {
  // TODO
}
#endif /* KLT_CUDA_ENABLED */

void klt_init(void) {
  klt_host_init();

#if KLT_THREADS_ENABLED
  klt_threads_init();
#endif /* KLT_THREADS_ENABLED */

#if KLT_OPENCL_ENABLED
  klt_opencl_init();
#endif /* KLT_OPENCL_ENABLED */

#if KLT_CUDA_ENABLED
  klt_cuda_init();
#endif /* KLT_CUDA_ENABLED */
}

void klt_host_exit(void);
void klt_host_exit(void) {
  // TODO
}

#if KLT_THREADS_ENABLED
void klt_threads_exit(void);
void klt_threads_exit(void) {
  // TODO
}
#endif /* KLT_THREADS_ENABLED */

#if KLT_OPENCL_ENABLED
void klt_opencl_exit(void);
void klt_opencl_exit(void) {
  size_t i;
  for (i = 0; i < klt_devices_count; i++)
    if (klt_devices[i]->kind == e_klt_opencl) {
      printf("klt_opencl_exit : device #%zd\n", i);
      clFinish(klt_devices[i]->descriptor.opencl->queue);
      klt_check_opencl_status("[Error] clReleaseContext returns:", clReleaseContext(klt_devices[i]->descriptor.opencl->context));
      klt_check_opencl_status("[Error] clReleaseCommandQueue returns:", clReleaseCommandQueue(klt_devices[i]->descriptor.opencl->queue));
      klt_check_opencl_status("[Error] clReleaseProgram returns:", clReleaseProgram(klt_devices[i]->descriptor.opencl->program));
    }
}
#endif /* KLT_OPENCL_ENABLED */

#if KLT_CUDA_ENABLED
void klt_cuda_exit(void);
void klt_cuda_exit(void) {
  // TODO
}
#endif /* KLT_CUDA_ENABLED */

void klt_exit(void) {
#if KLT_OPENCL_ENABLED
  klt_opencl_exit();
#endif /* KLT_OPENCL_ENABLED */

#if KLT_CUDA_ENABLED
  klt_cuda_exit();
#endif /* KLT_CUDA_ENABLED */

#if KLT_THREADS_ENABLED
  klt_threads_exit();
#endif /* KLT_THREADS_ENABLED */

  klt_host_exit();
}

