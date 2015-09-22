
#include "KLT/RTL/device.h"
#include "KLT/RTL/memory.h"
#include "KLT/RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/context.h"
#include "KLT/RTL/build-context.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct klt_version_desc_t * klt_user_select_kernel_version(struct klt_kernel_t * kernel, size_t num_candidates, struct klt_version_desc_t ** candidates) {
  assert(num_candidates == 1);
  return candidates[0];
}

struct klt_version_desc_t * iklt_select_kernel_version(struct klt_kernel_t * kernel, enum klt_device_e device_kind) {
  assert(kernel->desc->num_versions > 0);

  struct klt_version_desc_t ** candidates = malloc(kernel->desc->num_versions * sizeof(struct klt_version_desc_t *)); // over provision the allocation
  assert(candidates != NULL);

  size_t i, cnt = 0;
  for (i = 0; i < kernel->desc->num_versions; i++)
    if (kernel->desc->versions[i].device_kind == device_kind)
      candidates[cnt++] = &(kernel->desc->versions[i]);

  assert(cnt > 0);

  struct klt_version_desc_t * res = klt_user_select_kernel_version(kernel, cnt, candidates);
  assert(res != NULL);

  free(candidates);

  return res;
}

struct klt_kernel_t * klt_build_kernel(int idx) {
  struct klt_kernel_desc_t * desc = &(klt_kernel_desc[idx]);

  struct klt_kernel_t * res = (struct klt_kernel_t *)malloc(sizeof(struct klt_kernel_t));

  res->desc   = desc;

  res->param  = (void **)malloc(desc->data.num_param  * sizeof(void *));

  res->data   = (struct klt_data_t *)malloc(desc->data.num_data * sizeof(struct klt_data_t));

  res->loops  = (struct klt_loop_t *)malloc(desc->loop.num_loops * sizeof(struct klt_loop_t));

  res->num_threads = 0;
  res->num_gangs[0] = 0;
  res->num_gangs[1] = 0;
  res->num_gangs[2] = 0;
  res->num_workers[0] = 0;
  res->num_workers[1] = 0;
  res->num_workers[2] = 0;

  int i;
  for (i = 0; i < desc->data.num_data; i++) {
    res->data[i].base_type_size = desc->data.sizeof_data[i];
    res->data[i].num_sections = desc->data.ndims_data[i];
    res->data[i].sections = (struct klt_data_section_t *)malloc(desc->data.ndims_data[i] * sizeof(struct klt_data_section_t));
  }

  return res;
}



#if KLT_THREADS_ENABLED
struct klt_threads_workload_t * klt_create_threads_workload(
  klt_threads_kernel_func_ptr kernel_func,
  void ** local_param,
  void ** local_data,
  struct klt_loop_context_t * loop_context,
  struct klt_data_context_t * data_context
) {
  struct klt_threads_workload_t * workload = malloc(sizeof(struct klt_threads_workload_t));
    workload->kernel_func = kernel_func;
    workload->parameters = local_param;
    workload->data = local_data;
    workload->loop_context = klt_copy_loop_context(loop_context);
    workload->data_context = klt_copy_data_context(data_context);
  return workload;
}
#endif

void iklt_execute_subkernels(
  struct klt_kernel_t * kernel, struct klt_version_desc_t * version
) {
  assert(kernel->device_id < klt_devices_count);
  struct klt_device_t * device = klt_devices[kernel->device_id];
  assert(device != NULL);

  size_t i, j;
  for (i = 0; i < version->num_subkernels; i++) {
    struct klt_subkernel_desc_t * subkernel = &(version->subkernels[i]);

    // Check compatibility between subkernel and version (TODO complex cases)
    assert(subkernel->device_kind == version->device_kind);

    struct klt_loop_context_t * klt_loop_context = klt_build_loop_context(&(subkernel->loop), kernel->loops, kernel);
    struct klt_data_context_t * klt_data_context = klt_build_data_context();

    void ** local_param = (void **)malloc(subkernel->num_params * sizeof(void *));
    for (j = 0; j < subkernel->num_params; j++) {
      local_param[j] = kernel->param[subkernel->param_ids[j]];
    }

    void ** local_data = (void **)malloc(subkernel->num_data * sizeof(void *));
    for (j = 0; j < subkernel->num_data; j++) {
      struct klt_data_t * data = &(kernel->data[subkernel->data_ids[j]]);
      assert(data != NULL);
      struct klt_allocation_t * alloc = klt_get_data(data, kernel->device_id);
      assert(alloc != NULL);
      local_data[j] = alloc->descriptor;
    }

#if KLT_SUBKERNEL_TIMING
    struct timespec timer_start;
    struct timespec timer_stop;
#endif /* KLT_SUBKERNEL_TIMING */

    switch (subkernel->device_kind) {
      case e_klt_host:
      {
        assert(device->descriptor.host == NULL);

#if KLT_SUBKERNEL_TIMING
        clock_gettime(CLOCK_REALTIME, &timer_start);
#endif /* KLT_SUBKERNEL_TIMING */

        klt_host_kernel_func_ptr host_kernel_func = (klt_host_kernel_func_ptr)(subkernel->descriptor);
        (*host_kernel_func)(local_param, local_data, klt_loop_context, klt_data_context);

#if KLT_SUBKERNEL_TIMING
        clock_gettime(CLOCK_REALTIME, &timer_stop);
#endif /* KLT_SUBKERNEL_TIMING */
        break;
      }
      case e_klt_threads:
      {
#if KLT_THREADS_ENABLED
        assert(device->descriptor.threads != NULL);
        struct klt_threads_device_t * threads_device = device->descriptor.threads;

        size_t num_threads = kernel->num_threads;
        assert(num_threads <= threads_device->num_threads);

        struct klt_threads_workload_t ** workloads = malloc(num_threads * sizeof(struct klt_threads_workload_t *));
        size_t tid;
        for (tid = 0; tid < num_threads; tid++)
          workloads[tid] = klt_create_threads_workload((klt_threads_kernel_func_ptr)(subkernel->descriptor), local_param, local_data, klt_loop_context, klt_data_context);

#if KLT_SUBKERNEL_TIMING
        clock_gettime(CLOCK_REALTIME, &timer_start);
#endif /* KLT_SUBKERNEL_TIMING */

        for (tid = 0; tid < num_threads; tid++)
          klt_threads_submit_workload(threads_device, tid, workloads[tid]);

//      sleep(10);

#if KLT_SUBKERNEL_TIMING
        klt_threads_wait_for_completion(threads_device);
        clock_gettime(CLOCK_REALTIME, &timer_stop);
#endif /* KLT_SUBKERNEL_TIMING */
        free(workloads);

#else /* KLT_THREADS_ENABLED */
        assert(0); // Threads are not enables
#endif /* KLT_THREADS_ENABLED */
        break;
      }
      case e_klt_opencl:
      {
#if KLT_OPENCL_ENABLED
        assert(0); // NIY OpenCL
#else /* KLT_OPENCL_ENABLED */
        assert(0); // OpenCL is not enables
#endif /* KLT_OPENCL_ENABLED */
        break;
      }
      case e_klt_cuda:
      {
#if KLT_CUDA_ENABLED
        assert(0); // NIY CUDA
#else /* KLT_CUDA_ENABLED */
        assert(0); // CUDA is not enables
#endif /* KLT_CUDA_ENABLED */
        break;
      }
    }

#if KLT_SUBKERNEL_TIMING
    long timer_delta = (timer_stop.tv_nsec - timer_start.tv_nsec) / 1000000;
    if (timer_delta > 0)
      timer_delta += (timer_stop.tv_sec - timer_start.tv_sec) * 1000;
    else
      timer_delta = (timer_stop.tv_sec - timer_start.tv_sec) * 1000 - timer_delta;
    printf("%d", timer_delta);
#endif /* KLT_SUBKERNEL_TIMING */

    free(klt_loop_context);
    free(klt_data_context);
    free(local_param);
    free(local_data);
  }
}

void klt_execute_kernel(struct klt_kernel_t * kernel) {
  assert(kernel->device_id < klt_devices_count);
  struct klt_device_t * device = klt_devices[kernel->device_id];
  assert(device != NULL);

  enum klt_device_e device_kind = device->kind;

  struct klt_version_desc_t * version = iklt_select_kernel_version(kernel, device_kind);
  assert(version != NULL);
  assert(version->device_kind == device_kind);

#if KLT_KERNEL_TIMING
  struct timespec timer_start;
  struct timespec timer_stop;

  clock_gettime(CLOCK_REALTIME, &timer_start);
#endif /* KLT_KERNEL_TIMING */

  iklt_execute_subkernels(kernel, version);

  switch (version->device_kind) {
    case e_klt_host:
      break;
    case e_klt_threads:
#if KLT_THREADS_ENABLED
      klt_threads_wait_for_completion(device->descriptor.threads);
      break;
#else /* KLT_THREADS_ENABLED */
      assert(0); // Threads are not enables
#endif /* KLT_THREADS_ENABLED */
    case e_klt_opencl:
#if KLT_OPENCL_ENABLED
      assert(0); // NIY OpenCL
#else /* KLT_OPENCL_ENABLED */
      assert(0); // OpenCL is not enables
#endif /* KLT_OPENCL_ENABLED */
    case e_klt_cuda:
#if KLT_CUDA_ENABLED
      assert(0); // NIY CUDA
#else /* KLT_CUDA_ENABLED */
      assert(0); // CUDA is not enables
#endif /* KLT_CUDA_ENABLED */
  }

#if KLT_KERNEL_TIMING
    clock_gettime(CLOCK_REALTIME, &timer_stop);

    long timer_delta = (timer_stop.tv_nsec - timer_start.tv_nsec) / 1000000;
    if (timer_delta > 0)
      timer_delta += (timer_stop.tv_sec - timer_start.tv_sec) * 1000;
    else
      timer_delta = (timer_stop.tv_sec - timer_start.tv_sec) * 1000 - timer_delta;
    printf("%d", timer_delta);
#endif /* KLT_KERNEL_TIMING */
}

