
#include "KLT/RTL/device.h"
#include "KLT/RTL/memory.h"
#include "KLT/RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/context.h"
#include "KLT/RTL/build-context.h"
#include "KLT/RTL/data-environment.h"

#if KLT_OPENCL_ENABLED
#  include "KLT/RTL/opencl-utils.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef KLT_EXPORT_LOOP_CTX_TO_JSON
#define KLT_EXPORT_LOOP_CTX_TO_JSON 1
#endif

extern char * klt_file_stem;

struct klt_version_desc_t * klt_user_select_kernel_version(struct klt_kernel_t * kernel, size_t num_candidates, struct klt_version_desc_t ** candidates);
struct klt_version_desc_t * klt_user_select_kernel_version(struct klt_kernel_t * kernel, size_t num_candidates, struct klt_version_desc_t ** candidates) {
  assert(num_candidates == 1);
  return candidates[0];
}

struct klt_version_desc_t * iklt_select_kernel_version(struct klt_kernel_t * kernel, enum klt_device_e device_kind);
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

void ** klt_copy_pointers_array(size_t n, void ** arr);
void ** klt_copy_pointers_array(size_t n, void ** arr) {
  void ** res = (void **)malloc(n * sizeof(void *));
  memcpy(res, arr, n * sizeof(void *));
  return res;
}

#if KLT_THREADS_ENABLED
struct klt_threads_workload_t * klt_create_threads_workload(
  klt_threads_kernel_func_ptr kernel_func,
  size_t num_param, void ** local_param,
  size_t num_data,  void ** local_data,
  struct klt_loop_context_t * loop_context,
  struct klt_data_context_t * data_context
);
struct klt_threads_workload_t * klt_create_threads_workload(
  klt_threads_kernel_func_ptr kernel_func,
  size_t num_param, void ** local_param,
  size_t num_data,  void ** local_data,
  struct klt_loop_context_t * loop_context,
  struct klt_data_context_t * data_context
) {
  struct klt_threads_workload_t * workload = malloc(sizeof(struct klt_threads_workload_t));
    workload->kernel_func = kernel_func;
    workload->parameters = klt_copy_pointers_array(num_param, local_param);
    workload->data = klt_copy_pointers_array(num_data, local_data);
    workload->loop_context = klt_copy_loop_context(loop_context);
    workload->data_context = klt_copy_data_context(data_context);
  return workload;
}
#endif

void iklt_execute_subkernels(struct klt_kernel_t * kernel, struct klt_version_desc_t * version);
void iklt_execute_subkernels(struct klt_kernel_t * kernel, struct klt_version_desc_t * version) {
  assert(kernel->device_id < klt_devices_count);
  struct klt_device_t * device = klt_devices[kernel->device_id];
  assert(device != NULL);

  size_t i, j;
  for (i = 0; i < version->num_subkernels; i++) {
    struct klt_subkernel_desc_t * subkernel = &(version->subkernels[i]);

    // Check compatibility between subkernel and version (TODO complex cases)
    assert(subkernel->device_kind == version->device_kind);

    struct klt_loop_context_t * klt_loop_context = klt_build_loop_context(&(subkernel->loop), kernel->loops, kernel);

#if KLT_EXPORT_LOOP_CTX_TO_JSON
    {
      size_t k;
      char * file_ext = "json";
      size_t kernel_id = (kernel->desc - klt_kernel_desc)/sizeof(struct klt_kernel_desc_t);
//    size_t version_id = (version - kernel->desc->versions)/sizeof(struct klt_version_desc_t);
      size_t subkernel_id = i;
      size_t filename_length = strlen(klt_file_stem) + strlen(file_ext) + 40;

      char * filename = malloc(filename_length * sizeof(char));
      memset(filename, 0, filename_length * sizeof(char));
      sprintf(filename, "%s_kernel_%zd_subkernel_%zd_loop_ctx.%s", klt_file_stem, kernel_id, subkernel_id, file_ext);

      FILE * loop_ctx_json = fopen(filename, "w");

      int * ptr = klt_loop_context->data;

      fprintf(loop_ctx_json, "{\n");
      fprintf(loop_ctx_json, "  \"num_loops\": %d,\n", klt_loop_context->num_loops);
      fprintf(loop_ctx_json, "  \"loops\": [\n");
      fprintf(loop_ctx_json, "    { \"id\":0, \"lower\":%d, \"upper\":%d, \"stride\":%d }", *(ptr+0), *(ptr+1), *(ptr+2));
      ptr += 3;
      for (k = 1; k < klt_loop_context->num_loops; k++) {
        fprintf(loop_ctx_json, ",\n    { \"id\":%zd, \"lower\":%d, \"upper\":%d, \"stride\":%d }", k, *(ptr+0), *(ptr+1), *(ptr+2));
        ptr += 3;
      }
      fprintf(loop_ctx_json, "\n  ],\n");
      fprintf(loop_ctx_json, "  \"num_tiles\": %d,\n", klt_loop_context->num_tiles);
      fprintf(loop_ctx_json, "  \"tiles\": [\n");
      fprintf(loop_ctx_json, "    { \"id\":0, \"length\":%d, \"stride\":%d }", *(ptr+0), *(ptr+1));
      ptr += 2;
      for (k = 1; k < klt_loop_context->num_tiles; k++) {
        fprintf(loop_ctx_json, ",\n    { \"id\":%zd, \"length\":%d, \"stride\":%d }", k, *(ptr+0), *(ptr+1));
        ptr += 2;
      }
      fprintf(loop_ctx_json, "\n  ]\n");
      fprintf(loop_ctx_json, "}\n");

      fclose(loop_ctx_json);
    }
#endif

    struct klt_data_context_t * klt_data_context = klt_build_data_context(subkernel->num_data);

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

        klt_host_kernel_func_ptr host_kernel_func = (klt_host_kernel_func_ptr)(subkernel->descriptor.host);
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
        assert(threads_device != NULL);

        size_t num_threads = kernel->num_threads;
        assert(num_threads > 0);
        assert(num_threads <= threads_device->num_threads);

        struct klt_threads_workload_t ** workloads = malloc(num_threads * sizeof(struct klt_threads_workload_t *));
        size_t tid;
        for (tid = 0; tid < num_threads; tid++)
          workloads[tid] = klt_create_threads_workload((klt_threads_kernel_func_ptr)(subkernel->descriptor.threads), subkernel->num_params, local_param, subkernel->num_data, local_data, klt_loop_context, klt_data_context);

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
        cl_int status;
        assert(device->descriptor.opencl != NULL);

        struct klt_opencl_device_t * opencl_device = device->descriptor.opencl;

        // Allocate and copy loop & data contexts

        size_t size_loop_ctx = sizeof(struct klt_loop_context_t) + 3 * klt_loop_context->num_loops * sizeof(int) + 2 * klt_loop_context->num_tiles * sizeof(int);
        cl_mem ocl_loop_context = clCreateBuffer(opencl_device->context, CL_MEM_READ_ONLY, size_loop_ctx, NULL, NULL);

        status = clEnqueueWriteBuffer(opencl_device->queue, ocl_loop_context, CL_FALSE, 0, size_loop_ctx, klt_loop_context, 0, NULL, NULL);
        assert(status == CL_SUCCESS);

        size_t size_data_ctx = sizeof(struct klt_data_context_t);
        cl_mem ocl_data_context = clCreateBuffer(opencl_device->context, CL_MEM_READ_ONLY, size_data_ctx, NULL, NULL);

        status = clEnqueueWriteBuffer(opencl_device->queue, ocl_data_context, CL_FALSE, 0, size_data_ctx, klt_data_context, 0, NULL, NULL);
        assert(status == CL_SUCCESS);

        // Create OpenCL kernel

//      printf("clCreateKernel: %s\n", subkernel->descriptor.accelerator);

        cl_kernel ocl_kernel = clCreateKernel(opencl_device->program, subkernel->descriptor.accelerator, &status);
        klt_opencl_check(status, "clCreateKernel");

        // Set kernel arguments

        size_t arg_cnt = 0;
        for (i = 0; i < subkernel->num_params; i++) {
          status = clSetKernelArg(ocl_kernel, arg_cnt++, kernel->desc->data.sizeof_param[subkernel->param_ids[i]], local_param[i]);
          klt_opencl_check(status, "clSetKernelArg (for parameter)");
        }

        for (i = 0; i < subkernel->num_data; i++) {
          status = clSetKernelArg(ocl_kernel, arg_cnt++, sizeof(cl_mem), &(local_data[i]));
          klt_opencl_check(status, "clSetKernelArg (for data)");
        }

        status = clSetKernelArg(ocl_kernel, arg_cnt++, sizeof(cl_mem), &ocl_loop_context);
        klt_opencl_check(status, "clSetKernelArg (for loop context)");

        status = clSetKernelArg(ocl_kernel, arg_cnt++, sizeof(cl_mem), &ocl_data_context);
        klt_opencl_check(status, "clSetKernelArg (for data context)");

        // Contexts (loop and data) are freed at the end of this function so we need to wait for them to be copied
        clFinish(opencl_device->queue);

        // Launch kernel

        size_t global_work_size[3] = {
                                       kernel->num_gangs[0] * kernel->num_workers[0],
                                       kernel->num_gangs[1] * kernel->num_workers[1],
                                       kernel->num_gangs[2] * kernel->num_workers[2]
                                     };
        size_t local_work_size[3] =  {
                                       kernel->num_workers[0],
                                       kernel->num_workers[1],
                                       kernel->num_workers[2]
                                     };

//      printf("global_work_size = { %d , %d , %d }\n", global_work_size[0], global_work_size[1], global_work_size[2]);
//      printf("local_work_size  = { %d , %d , %d }\n", local_work_size [0], local_work_size [1], local_work_size [2]);

        status = clEnqueueNDRangeKernel(opencl_device->queue, ocl_kernel, 3, NULL, global_work_size, local_work_size, 0, NULL, NULL);
        klt_opencl_check(status, "clEnqueueNDRangeKernel");

        { /// FIXME create a list of mem object to be free'd
          clFinish(opencl_device->queue);
          clReleaseKernel(ocl_kernel);
          clReleaseMemObject(ocl_loop_context);
          clReleaseMemObject(ocl_data_context);
        }

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

//printf("Execute kernel on device #%d\n", kernel->device_id);

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
    {
      /* NOP */
      break;
    }
    case e_klt_threads:
    {
#if KLT_THREADS_ENABLED
      klt_threads_wait_for_completion(device->descriptor.threads);
      break;
#else /* KLT_THREADS_ENABLED */
      assert(0); // Threads are not enables
#endif /* KLT_THREADS_ENABLED */
    }
    case e_klt_opencl:
    {
#if KLT_OPENCL_ENABLED
      clFinish(device->descriptor.opencl->queue);
      break;
#else /* KLT_OPENCL_ENABLED */
      assert(0); // OpenCL is not enables
#endif /* KLT_OPENCL_ENABLED */
    }
    case e_klt_cuda:
    {
#if KLT_CUDA_ENABLED
      assert(0); // NIY CUDA
#else /* KLT_CUDA_ENABLED */
      assert(0); // CUDA is not enables
#endif /* KLT_CUDA_ENABLED */
    }
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

void klt_dbg_dump_kernel(size_t idx) {
  size_t i, j, k, l;

  printf("*\n");
  printf("* Kernel #%zd\n", idx);
  printf("*\n");

  printf("* [%zd].num_parameters = %zd\n", idx, klt_kernel_desc[idx].data.num_param);
  for (i = 0; i < klt_kernel_desc[idx].data.num_param; i++) {
    printf("* [%zd][%zd].size = %zd\n", idx, i, klt_kernel_desc[idx].data.sizeof_param[i]);
  }

  printf("*\n");

  printf("* [%zd].num_data = %zd\n", idx, klt_kernel_desc[idx].data.num_data);
  for (i = 0; i < klt_kernel_desc[idx].data.num_data; i++) {
    printf("* [%zd][%zd].ndim = %zd\n", idx, i, klt_kernel_desc[idx].data.sizeof_data[i]);
    printf("* [%zd][%zd].size = %zd\n", idx, i, klt_kernel_desc[idx].data.ndims_data[i]);
  }

  printf("*\n");

  printf("* [%zd].num_loops = %d\n", idx, klt_kernel_desc[idx].loop.num_loops);
  for (i = 0; i < klt_kernel_desc[idx].loop.num_loops; i++) {
    printf("* [%zd][%zd].idx = %d\n", idx, i, klt_kernel_desc[idx].loop.loop_desc[i].idx);
  }

  printf("*\n");

  printf("* [%zd].num_versions = %d\n", idx, klt_kernel_desc[idx].num_versions);
  printf("* [%zd].versions :\n", idx);
  for (i = 0; i < klt_kernel_desc[idx].num_versions; i++) {
    printf("* [%zd][%zd].target = %u\n", idx, i, klt_kernel_desc[idx].versions[i].device_kind);
    // TODO struct klt_version_selector_t version_selector;

    printf("* [%zd][%zd].num_subkernels = %d\n", idx, i, klt_kernel_desc[idx].versions[i].num_subkernels);
    printf("* [%zd][%zd].subkernels :\n"       , idx, i);
    for (j = 0; j < klt_kernel_desc[idx].versions[i].num_subkernels; j++) {
      printf("* [%zd][%zd][%zd].id        = %zd\n", idx, i, j, klt_kernel_desc[idx].versions[i].subkernels[j].id);
      printf("* [%zd][%zd][%zd].target    = %d\n",  idx, i, j, klt_kernel_desc[idx].versions[i].subkernels[j].device_kind);
      printf("* [%zd][%zd][%zd].num_loops = %d\n",  idx, i, j, klt_kernel_desc[idx].versions[i].subkernels[j].loop.num_loops);
      printf("* [%zd][%zd][%zd].num_tiles = %d\n",  idx, i, j, klt_kernel_desc[idx].versions[i].subkernels[j].loop.num_tiles);
      printf("* [%zd][%zd][%zd].loop_desc :\n"   ,  idx, i, j);
      for (k = 0; k < klt_kernel_desc[idx].versions[i].subkernels[j].loop.num_loops; k++) {
        printf("* [%zd][%zd][%zd][%zd].idx       = %d\n", idx, i, j, k, klt_kernel_desc[idx].versions[i].subkernels[j].loop.loop_desc[k].idx);
        printf("* [%zd][%zd][%zd][%zd].num_tiles = %d\n", idx, i, j, k, klt_kernel_desc[idx].versions[i].subkernels[j].loop.loop_desc[k].num_tiles);
        printf("* [%zd][%zd][%zd][%zd].tile_desc :\n"   , idx, i, j, k);
        for (l = 0; l < klt_kernel_desc[idx].versions[i].subkernels[j].loop.loop_desc[k].num_tiles; l++) {
          printf("* [%zd][%zd][%zd][%zd][%zd].idx   = %d\n", idx, i, j, k, l, klt_kernel_desc[idx].versions[i].subkernels[j].loop.loop_desc[k].tile_desc[l].idx);
          printf("* [%zd][%zd][%zd][%zd][%zd].kind  = %u\n", idx, i, j, k, l, klt_kernel_desc[idx].versions[i].subkernels[j].loop.loop_desc[k].tile_desc[l].kind);
          printf("* [%zd][%zd][%zd][%zd][%zd].param = %d\n", idx, i, j, k, l, klt_kernel_desc[idx].versions[i].subkernels[j].loop.loop_desc[k].tile_desc[l].param);
        }
      }
    }
  }

  printf("*\n");
}

void klt_dbg_dump_all_kernels(void) {
  size_t i;
  printf("*******\n");
  for (i = 0; i < klt_num_kernels; i++) {
    klt_dbg_dump_kernel(i);
    printf("*******\n");
  }
}

