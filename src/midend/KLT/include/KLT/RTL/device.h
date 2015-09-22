
#ifndef __KLT_RTL_DEVICE_H__
#define __KLT_RTL_DEVICE_H__

#include "KLT/RTL/definition.h"

#if KLT_THREADS_ENABLED
#include <pthread.h>
struct klt_threads_workload_t;
#endif

#if KLT_OPENCL_ENABLED
#include <CL/opencl.h>
#endif

struct klt_memloc_t;

#if KLT_THREADS_ENABLED
struct klt_threads_t {
  size_t tid;

  pthread_t pthread;

  pthread_mutex_t work_mutex;
  pthread_cond_t work_cond;

  pthread_mutex_t done_mutex;
  pthread_cond_t done_cond;

  struct klt_threads_workload_t * workload;

  int finished;
};

struct klt_threads_device_t {
  size_t num_threads;
  struct klt_threads_t * threads;
};

struct klt_threads_device_t * iklt_build_threads_device(size_t num_threads);
#endif /* KLT_THREADS_ENABLED */

#if KLT_OPENCL_ENABLED
struct klt_opencl_device_t {
  cl_platform_id platform;
  cl_device_id device;
  cl_context context;
  cl_command_queue queue;
  cl_program program;
};

struct klt_opencl_device_t * klt_build_opencl_device(cl_platform_id platform, cl_device_id device, size_t num_sources, char ** sources, char * options);
#endif /* KLT_OPENCL_ENABLED */

struct klt_device_t {
  size_t device_id;
  enum klt_device_e kind;

  struct klt_device_t * parent;

  size_t num_subdevices;
  struct klt_device_t * subdevices;

  size_t num_memlocs;
  struct klt_memloc_t * memlocs;

  union {
    void * host;
#if KLT_THREADS_ENABLED
    struct klt_threads_device_t * threads;
#endif
#if KLT_OPENCL_ENABLED
    struct klt_opencl_device_t * opencl;
#endif
    // TODO OpenCL
    // TODO CUDA
  } descriptor;
};

extern size_t klt_devices_count;
extern struct klt_device_t ** klt_devices;

size_t iklt_increase_alloc_devices();
size_t iklt_device_increase_alloc_subdevices(struct klt_device_t * device);
size_t iklt_device_increase_alloc_memlocs(struct klt_device_t * device);

struct klt_device_t * klt_get_device_by_id(size_t id);

struct klt_memloc_t * klt_get_matching_memloc(size_t device_id, enum klt_memory_mode_e mode);

struct klt_device_t * klt_create_subdevice(size_t parent_id, enum klt_device_e kind);

#endif /* __KLT_RTL_DEVICE_H__ */

