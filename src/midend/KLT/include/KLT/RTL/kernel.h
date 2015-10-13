
#ifndef __KLT_RTL_KERNEL_H__
#define __KLT_RTL_KERNEL_H__

#include "KLT/RTL/definition.h"

#include "KLT/RTL/loop.h"
#include "KLT/RTL/data.h"

struct klt_version_selector_t {};

#if KLT_THREADS_ENABLED
struct klt_threads_workload_t {
  klt_threads_kernel_func_ptr kernel_func;
  void ** parameters;
  void ** data;
  struct klt_loop_context_t * loop_context;
  struct klt_data_context_t * data_context;
};
#endif

struct klt_subkernel_desc_t {
  size_t id;
  enum klt_device_e device_kind;
  struct klt_loop_container_t loop;
  int num_params;
  int * param_ids;
  int num_data;
  int * data_ids;
  int num_loops;
  int * loop_ids;
  int num_deps;
  int * deps_ids;
  union subkernel_descriptor_u {
    klt_host_kernel_func_ptr host;
#if KLT_THREADS_ENABLED
    klt_threads_kernel_func_ptr threads;
#endif
#if KLT_OPENCL_ENABLED || KLT_CUDA_ENABLED
    char * accelerator;
#endif
  } descriptor;
};

struct klt_version_desc_t {
  enum klt_device_e device_kind;
  struct klt_version_selector_t version_selector;

  int num_subkernels;
  struct klt_subkernel_desc_t * subkernels;
};

struct klt_kernel_desc_t {
  struct klt_data_container_t data;
  struct klt_loop_container_t loop;

  int num_versions;
  struct klt_version_desc_t * versions;
};

struct klt_kernel_t {
  struct klt_kernel_desc_t * desc;

  size_t device_id;

  void ** param;
  struct klt_data_t * data;

  struct klt_loop_t * loops;

  size_t num_threads;

  size_t num_gangs[3];
  size_t num_workers[3];
};

extern size_t klt_num_kernels;
extern struct klt_kernel_desc_t klt_kernel_desc[];

struct klt_kernel_t * klt_build_kernel(int idx);

void klt_execute_kernel(struct klt_kernel_t * kernel);

void klt_dbg_dump_kernel(int idx);

void klt_dbg_dump_all_kernels();

#endif /* __KLT_RTL_KERNEL_H__ */

