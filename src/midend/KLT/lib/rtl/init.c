
#include "KLT/RTL/init.h"

#include "KLT/RTL/memory.h"
#include "KLT/RTL/device.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>

void klt_host_init() {
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
void klt_threads_init() {
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

char * read_file(const char * filename);
void dbg_get_ocl_build_log(cl_device_id device, cl_program program);

void klt_opencl_init() {
  size_t i, j;
  cl_int status;

  // Sources and Options

  char * sources[2] = { read_file(opencl_kernel_file) , read_file(opencl_klt_runtime_lib) };

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
}
#endif /* KLT_OPENCL_ENABLED */

#if KLT_CUDA_ENABLED
void klt_cuda_init() {
  // TODO
}
#endif /* KLT_CUDA_ENABLED */

void klt_init() {
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

#if KLT_OPENCL_ENABLED
char * read_file(const char * filename) {

   FILE *fp;
   int err;
   int size;

   char *source;

   fp = fopen(filename, "rb");
   if(fp == NULL) {
      printf("Could not open kernel file: %s\n", filename);
      assert(0);
   }
   
   err = fseek(fp, 0, SEEK_END);
   if(err != 0) {
      printf("Error seeking to end of file\n");
      assert(0);
   }

   size = ftell(fp);
   if(size < 0) {
      printf("Error getting file position\n");
      assert(0);
   }

   err = fseek(fp, 0, SEEK_SET);
   if(err != 0) {
      printf("Error seeking to start of file\n");
      assert(0);
   }

   source = (char*)malloc(size+1);
   if(source == NULL) {
      printf("Error allocating %d bytes for the program source\n", size+1);
      assert(0);
   }

   err = fread(source, 1, size, fp);
   if(err != size) {
      printf("only read %d bytes\n", err);
      assert(0);
   }

   source[size] = '\0';

   return source;
}

void dbg_get_ocl_build_log(cl_device_id device, cl_program program) {
  char * build_log;
  size_t build_log_size;
  clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_size);
  if (build_log_size == 0)
    printf("[warning] OpenCL return an empty log...\n");
  else {
    build_log = (char*)malloc(build_log_size);
    if (build_log == NULL) {
      perror("[fatal] malloc : build_log");
      exit(-1);
    }
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, build_log_size, build_log, NULL);
    build_log[build_log_size-1] = '\0';
    printf("*********************************\n%s\n*********************************\n", build_log);
    free(build_log);
  }
}

const char * ocl_status_to_char(int status) {
  switch (status) {
      case CL_DEVICE_NOT_FOUND:                          return (char *)"CL_DEVICE_NOT_FOUND";
      case CL_DEVICE_NOT_AVAILABLE:                      return (char *)"CL_DEVICE_NOT_AVAILABLE";
      case CL_COMPILER_NOT_AVAILABLE:                    return (char *)"CL_COMPILER_NOT_AVAILABLE";
      case CL_MEM_OBJECT_ALLOCATION_FAILURE:             return (char *)"CL_MEM_OBJECT_ALLOCATION_FAILURE";
      case CL_OUT_OF_RESOURCES:                          return (char *)"CL_OUT_OF_RESOURCES";
      case CL_OUT_OF_HOST_MEMORY:                        return (char *)"CL_OUT_OF_HOST_MEMORY";
      case CL_PROFILING_INFO_NOT_AVAILABLE:              return (char *)"CL_PROFILING_INFO_NOT_AVAILABLE";
      case CL_MEM_COPY_OVERLAP:                          return (char *)"CL_MEM_COPY_OVERLAP";
      case CL_IMAGE_FORMAT_MISMATCH:                     return (char *)"CL_IMAGE_FORMAT_MISMATCH";
      case CL_IMAGE_FORMAT_NOT_SUPPORTED:                return (char *)"CL_IMAGE_FORMAT_NOT_SUPPORTED";
      case CL_BUILD_PROGRAM_FAILURE:                     return (char *)"CL_BUILD_PROGRAM_FAILURE";
      case CL_MAP_FAILURE:                               return (char *)"CL_MAP_FAILURE";
      case CL_INVALID_VALUE:                             return (char *)"CL_INVALID_VALUE";
      case CL_INVALID_DEVICE_TYPE:                       return (char *)"CL_INVALID_DEVICE_TYPE";
      case CL_INVALID_PLATFORM:                          return (char *)"CL_INVALID_PLATFORM";
      case CL_INVALID_DEVICE:                            return (char *)"CL_INVALID_DEVICE";
      case CL_INVALID_CONTEXT:                           return (char *)"CL_INVALID_CONTEXT";
      case CL_INVALID_QUEUE_PROPERTIES:                  return (char *)"CL_INVALID_QUEUE_PROPERTIES";
      case CL_INVALID_COMMAND_QUEUE:                     return (char *)"CL_INVALID_COMMAND_QUEUE";
      case CL_INVALID_HOST_PTR:                          return (char *)"CL_INVALID_HOST_PTR";
      case CL_INVALID_MEM_OBJECT:                        return (char *)"CL_INVALID_MEM_OBJECT";
      case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:           return (char *)"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
      case CL_INVALID_IMAGE_SIZE:                        return (char *)"CL_INVALID_IMAGE_SIZE";
      case CL_INVALID_SAMPLER:                           return (char *)"CL_INVALID_SAMPLER";
      case CL_INVALID_BINARY:                            return (char *)"CL_INVALID_BINARY";
      case CL_INVALID_BUILD_OPTIONS:                     return (char *)"CL_INVALID_BUILD_OPTIONS";
      case CL_INVALID_PROGRAM:                           return (char *)"CL_INVALID_PROGRAM";
      case CL_INVALID_PROGRAM_EXECUTABLE:                return (char *)"CL_INVALID_PROGRAM_EXECUTABLE";
      case CL_INVALID_KERNEL_NAME:                       return (char *)"CL_INVALID_KERNEL_NAME";
      case CL_INVALID_KERNEL_DEFINITION:                 return (char *)"CL_INVALID_KERNEL_DEFINITION";
      case CL_INVALID_KERNEL:                            return (char *)"CL_INVALID_KERNEL";
      case CL_INVALID_ARG_INDEX:                         return (char *)"CL_INVALID_ARG_INDEX";
      case CL_INVALID_ARG_VALUE:                         return (char *)"CL_INVALID_ARG_VALUE";
      case CL_INVALID_ARG_SIZE:                          return (char *)"CL_INVALID_ARG_SIZE";
      case CL_INVALID_KERNEL_ARGS:                       return (char *)"CL_INVALID_KERNEL_ARGS";
      case CL_INVALID_WORK_DIMENSION:                    return (char *)"CL_INVALID_WORK_DIMENSION";
      case CL_INVALID_WORK_GROUP_SIZE:                   return (char *)"CL_INVALID_WORK_GROUP_SIZE";
      case CL_INVALID_WORK_ITEM_SIZE:                    return (char *)"CL_INVALID_WORK_ITEM_SIZE";
      case CL_INVALID_GLOBAL_OFFSET:                     return (char *)"CL_INVALID_GLOBAL_OFFSET";
      case CL_INVALID_EVENT_WAIT_LIST:                   return (char *)"CL_INVALID_EVENT_WAIT_LIST";
      case CL_INVALID_EVENT:                             return (char *)"CL_INVALID_EVENT";
      case CL_INVALID_OPERATION:                         return (char *)"CL_INVALID_OPERATION";
      case CL_INVALID_GL_OBJECT:                         return (char *)"CL_INVALID_GL_OBJECT";
      case CL_INVALID_BUFFER_SIZE:                       return (char *)"CL_INVALID_BUFFER_SIZE";
      case CL_INVALID_MIP_LEVEL:                         return (char *)"CL_INVALID_MIP_LEVEL";
      case CL_INVALID_GLOBAL_WORK_SIZE:                  return (char *)"CL_INVALID_GLOBAL_WORK_SIZE";
#ifdef CL_VERSION_1_1
      case CL_MISALIGNED_SUB_BUFFER_OFFSET:              return (char *)"CL_MISALIGNED_SUB_BUFFER_OFFSET";
      case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: return (char *)"CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
      case CL_INVALID_PROPERTY:                          return (char *)"CL_INVALID_PROPERTY";
#endif
      default:                                           return (char *)"CL_UNKNOWN_ERROR_CODE";
    }
  return (char *)"CL_UNKNOWN_ERROR_CODE";
}
#endif /* KLT_CUDA_ENABLED */

