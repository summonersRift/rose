
#include "KLT/RTL/init.h"
#include "KLT/RTL/io.h"

#include "KLT/RTL/memory.h"
#include "KLT/RTL/device.h"

#if KLT_OPENCL_ENABLED
#  include "KLT/RTL/opencl-utils.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>

extern char * klt_file_stem;
char * klt_runtime_incpath = NULL;
char * klt_runtime_libdir = NULL;

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

char * klt_opencl_runtime_lib = "rtl/context.c";

char * klt_opencl_kernel_dir = NULL;
char * klt_opencl_kernel_suffix = "-opencl-kernel.cl";

void klt_opencl_init(void);
void klt_opencl_init(void) {
  size_t i, j;

  // Load environment variables

  klt_opencl_kernel_dir = klt_getenv("KLT_OPENCL_KERNEL_DIR", "./");

  // Sources and Options

  char * options = NULL;
  char * sources[2] = { NULL , NULL };
  size_t num_sources = 0;

  if (klt_file_stem != NULL && klt_file_stem[0] != '\0') {
    size_t length = strlen(klt_opencl_kernel_dir) + strlen(klt_file_stem) + strlen(klt_opencl_kernel_suffix) + 20; // 20 for margin...
    char * filename = malloc(length);
    memset(filename, 0, length * sizeof(char));
    strcat(filename, klt_opencl_kernel_dir);
    strcat(filename, "/");
    strcat(filename, klt_file_stem);
    strcat(filename, klt_opencl_kernel_suffix);

    char * source = klt_read_file(filename);
    if (source != NULL) sources[num_sources++] = source;
  }
  {
    size_t length = strlen(klt_runtime_libdir) + strlen(klt_opencl_runtime_lib) + 20; // 20 for margin...
    char * filename = malloc(length);
    memset(filename, 0, length * sizeof(char));
    strcat(filename, klt_runtime_libdir);
    strcat(filename, "/");
    strcat(filename, klt_opencl_runtime_lib);

    char * source = klt_read_file(filename);
    if (source != NULL) sources[num_sources++] = source;
  }
  {
    char * context_storage_modifier = " -DCOMPILE_FOR_KERNEL=1 -DSTORAGE_MODIFIER=__constant -DDEVICE_FUNCTION_MODIFIER=";

    size_t length = strlen(klt_runtime_incpath) + strlen(context_storage_modifier) + 20; // 20 for margin...
#if COMPILE_OPENCL_KERNEL_WITH_DEBUG == 1
    char * debug_flags = " -g";
    length += strlen(debug_flags);
#endif

    options = (char *)malloc(length * sizeof(char));
    memset(options, 0, length * sizeof(char));

    strcat(options, klt_runtime_incpath);
    strcat(options, context_storage_modifier);
#if COMPILE_OPENCL_KERNEL_WITH_DEBUG == 1
    strcat(options, debug_flags);
#endif
  }

/*{
    printf("*******************************************\n");
    size_t i;
    printf("// %d input sources\n", num_sources);
    for (i = 0; i < num_sources; i++) {
      printf("// sources[%zd]:\n", i);
      printf("%s\n", sources[i]);
    }
    printf("*******************************************\n");
    printf("options: \"%s\"\n", options);
    printf("*******************************************\n");
  }*/

  // Iterate over OpenCL's Platforms & Devices

  cl_uint num_platforms;
  klt_opencl_assert(clGetPlatformIDs(0, NULL, &num_platforms), "clGetPlatformIDs (number)");

  cl_platform_id * platforms = (cl_platform_id *)malloc(num_platforms * sizeof(cl_platform_id));
  if (platforms == NULL)
    klt_fatal("Cannot allocate %d bytes!", num_platforms * sizeof(cl_platform_id));

  klt_opencl_assert(clGetPlatformIDs(num_platforms, platforms, NULL), "clGetPlatformIDs (platforms)");

  for (i = 0; i < num_platforms; i++) {
    cl_uint num_devices = 0;

    klt_opencl_assert(clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices), "clGetDeviceIDs (numbers)");

    cl_device_id * devices = (cl_device_id *)malloc(num_devices * sizeof(cl_device_id));
    assert(devices != NULL);

    klt_opencl_assert(clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_devices, devices, NULL), "clGetDeviceIDs (devices)");

    for (j = 0; j < num_devices; j++) {
      size_t device_idx, device_id, memloc_idx, memloc_id;

      /// platforms[i] / devices[j]
      struct klt_opencl_device_t * opencl_device = klt_build_opencl_device(platforms[i], devices[j], num_sources, (const char **)sources, options);

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
  {
    free(platforms);
    size_t i;
    for (i = 0; i < num_sources; i++)
      free(sources[i]);
    free(options);
  }
}
#endif /* KLT_OPENCL_ENABLED */

#if KLT_CUDA_ENABLED
void klt_cuda_init(void);
void klt_cuda_init(void) {
  // TODO
}
#endif /* KLT_CUDA_ENABLED */

void klt_init(void) {
  klt_init_io();

  klt_log("KLT starts...");

  klt_runtime_incpath = klt_getenv_or_fail("KLT_INCPATH");
  klt_runtime_libdir = klt_getenv_or_fail("KLT_LIBDIR");

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
      clFinish(klt_devices[i]->descriptor.opencl->queue);
      if (klt_devices[i]->descriptor.opencl->context != NULL)
        klt_opencl_check(clReleaseContext(klt_devices[i]->descriptor.opencl->context), "clReleaseContext");
      if (klt_devices[i]->descriptor.opencl->queue != NULL)
        klt_opencl_check(clReleaseCommandQueue(klt_devices[i]->descriptor.opencl->queue), "clReleaseCommandQueue");
      if (klt_devices[i]->descriptor.opencl->program != NULL)
        klt_opencl_check(clReleaseProgram(klt_devices[i]->descriptor.opencl->program), "clReleaseProgram");
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

  klt_log("KLT stops...");

  klt_exit_io();
}

