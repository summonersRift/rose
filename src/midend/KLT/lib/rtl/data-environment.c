
#include "KLT/RTL/data-environment.h"
#include "KLT/RTL/device.h"
#include "KLT/RTL/memory.h"
#include "KLT/RTL/data.h"

#if KLT_OPENCL_ENABLED
#  include "KLT/RTL/opencl-utils.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

struct iklt_alloc_map_t {
  size_t memloc_id;
  struct klt_allocation_t * allocation;
};

struct iklt_data_map_t {
  struct klt_data_t * data;
  size_t num_allocations;
  struct iklt_alloc_map_t * allocations;
};

struct klt_data_environment_t {
  struct klt_data_environment_t * parent;

  size_t num_data;
  struct iklt_data_map_t * allocations;
};

struct klt_data_environment_t * klt_data_environment = NULL;

///

struct klt_data_environment_t * klt_build_data_environment(struct klt_data_environment_t * parent);
struct klt_data_environment_t * klt_build_data_environment(struct klt_data_environment_t * parent) {
  struct klt_data_environment_t * res = malloc(sizeof(struct klt_data_environment_t));

  res->parent = parent;
  res->num_data = 0;
  res->allocations = NULL;

  return res;
}

void klt_clear_data_environment(struct klt_data_environment_t * data_env);
void klt_clear_data_environment(struct klt_data_environment_t * data_env) {
  size_t i, j;
  for (i = 0; i < data_env->num_data; i++) {
    for (j = 0; j < data_env->allocations[i].num_allocations; j++) {
      struct klt_memloc_t * memloc = klt_get_memloc_by_id(data_env->allocations[i].allocations[j].memloc_id);
      struct klt_device_t * device = klt_get_device_by_id(memloc->device_id);
      switch (device->kind) {
        case e_klt_host:
        {
          // NOP
          break;
        }
        case e_klt_threads:
        {
#if KLT_THREADS_ENABLED
          // NOP
          break;
#else /* KLT_THREADS_ENABLED */
          assert(0); // Threads are not enables
#endif /* KLT_THREADS_ENABLED */
        }
        case e_klt_opencl:
        {
#if KLT_OPENCL_ENABLED
          cl_int status;
          if (data_env->allocations[i].data->liveness == e_klt_live_out || data_env->allocations[i].data->liveness == e_klt_live_inout) {
            // if multiple allocation of one data, we cannot ensure which device allocation is written in original ptr !!! 
            status = clEnqueueReadBuffer(
              device->descriptor.opencl->queue,
              (cl_mem)data_env->allocations[i].allocations[j].allocation->descriptor,
              CL_TRUE,
              0,
              data_env->allocations[i].allocations[j].allocation->size,
              data_env->allocations[i].data->ptr,
              0,
              NULL,
              NULL
            ); /* TODO events chain */
            assert(status == CL_SUCCESS);
            clFinish(device->descriptor.opencl->queue);
          }
          status = clReleaseMemObject((cl_mem)data_env->allocations[i].allocations[j].allocation->descriptor);
          assert(status == CL_SUCCESS);
          break;
#else /* KLT_OPENCL_ENABLED */
          assert(0); // OpenCL is not enables
#endif /* KLT_OPENCL_ENABLED */
        }
        case e_klt_cuda:
        {
#if KLT_CUDA_ENABLED
          assert(0); // NIY CUDA
          if (data_env->allocations[i].data->liveness == e_klt_live_out || data_env->allocations[i].data->liveness == e_klt_live_inout) {
            // TODO copy the data back to the host (if multiple allocation of one data, behaviour is undefined)
          }
          // TODO klt_user_free_data_on_device(data_env->allocations[i].data, data_env->allocations[i].allocations[j].memloc, data_env->allocations[i].allocations[j].allocation);
          break;
#else /* KLT_CUDA_ENABLED */
          assert(0); // CUDA is not enables
#endif /* KLT_CUDA_ENABLED */
        }
      }
      free(data_env->allocations[i].allocations[j].allocation);
      data_env->allocations[i].allocations[j].memloc_id = -1;
      data_env->allocations[i].allocations[j].allocation = NULL;
    }
    data_env->allocations[i].data = NULL;
    data_env->allocations[i].num_allocations = 0;
    free(data_env->allocations[i].allocations);
    data_env->allocations[i].allocations = NULL;
  }
  data_env->parent = NULL;
  data_env->num_data = 0;
  free(data_env->allocations);
  data_env->allocations = NULL;
}

///

void klt_push_data_environment(void) {
  klt_data_environment = klt_build_data_environment(klt_data_environment);
}

void klt_pop_data_environment(void) {
  struct klt_data_environment_t * tmp = klt_data_environment;
  klt_data_environment = klt_data_environment->parent;
  klt_clear_data_environment(tmp);
}

///

struct iklt_data_map_t * iklt_get_data_map(struct klt_data_environment_t * data_env, struct klt_data_t * data);
struct iklt_data_map_t * iklt_get_data_map(struct klt_data_environment_t * data_env, struct klt_data_t * data) {
  size_t i;
  for (i = 0; i < data_env->num_data; i++)
    if (klt_same_data(data_env->allocations[i].data, data) == 0)
      return &(data_env->allocations[i]);
  return NULL;
}

struct iklt_data_map_t * iklt_lookup_data_deep(struct klt_data_t * data, struct klt_data_environment_t ** data_env_ptr);
struct iklt_data_map_t * iklt_lookup_data_deep(struct klt_data_t * data, struct klt_data_environment_t ** data_env_ptr) {
  assert(data_env_ptr != &klt_data_environment); // just to make sure

  while (*data_env_ptr != NULL) {
    struct iklt_data_map_t * data_map = iklt_get_data_map(*data_env_ptr, data);
    if (data_map != NULL) return data_map;
    *data_env_ptr = (*data_env_ptr)->parent;
  }
  return NULL;
}

struct iklt_data_map_t * iklt_lookup_data(struct klt_data_t * data);
struct iklt_data_map_t * iklt_lookup_data(struct klt_data_t * data) {
  struct klt_data_environment_t * data_env = klt_data_environment;
  return iklt_lookup_data_deep(data, &data_env);
}

struct klt_allocation_t * iklt_get_data_from_map(struct klt_data_t * data, struct klt_memloc_t * memloc, struct iklt_data_map_t * data_map);
struct klt_allocation_t * iklt_get_data_from_map(struct klt_data_t * data, struct klt_memloc_t * memloc, struct iklt_data_map_t * data_map) {
  size_t i;
  for (i = 0; i < data_map->num_allocations; i++)
    if (data_map->allocations[i].memloc_id == memloc->memloc_id)
      return data_map->allocations[i].allocation;

  return NULL;
}

struct klt_allocation_t * klt_get_data(struct klt_data_t * data, size_t device_id) {
  struct iklt_data_map_t * data_map = iklt_lookup_data(data);
  if (data_map == NULL) return NULL;

  struct klt_memloc_t * memloc = klt_get_memloc_by_id(klt_get_matching_memloc(device_id, data->mode));
  assert(memloc != NULL);

  return iklt_get_data_from_map(data, memloc, data_map);
}

///

struct iklt_data_map_t * iklt_declare_data(struct klt_data_t * data);
struct iklt_data_map_t * iklt_declare_data(struct klt_data_t * data) {
  klt_data_environment->num_data++;
  klt_data_environment->allocations = realloc(klt_data_environment->allocations, klt_data_environment->num_data * sizeof(struct iklt_data_map_t));
  assert(klt_data_environment->allocations != NULL);

  struct iklt_data_map_t * data_map = &(klt_data_environment->allocations[klt_data_environment->num_data - 1]);
    data_map->data = data;
    data_map->num_allocations = 1;
    data_map->allocations = malloc(sizeof(struct iklt_alloc_map_t));
    data_map->allocations[0].memloc_id = klt_get_matching_memloc(0, e_klt_read_write);
    data_map->allocations[0].allocation = malloc(sizeof(struct klt_allocation_t));
    data_map->allocations[0].allocation->mode = data->mode;
    data_map->allocations[0].allocation->size = 0;
    data_map->allocations[0].allocation->descriptor = data->ptr;
  return data_map;
}

void klt_allocate_data(struct klt_data_t * data, size_t device_id) {
  assert(data != NULL);

  struct klt_data_environment_t * data_env = klt_data_environment;
  assert(data_env != NULL);

  struct klt_memloc_t * memloc = klt_get_memloc_by_id(klt_get_matching_memloc(device_id, data->mode));
  assert(memloc != NULL);

  // Look for existing allocation
  struct iklt_data_map_t * data_map = NULL;
  while (data_env != NULL) {
    data_map = iklt_lookup_data_deep(data, &data_env);
    if (data_map == NULL) break;

    struct klt_allocation_t * alloc = iklt_get_data_from_map(data, memloc, data_map);
    if (alloc != NULL) return;
  }

  // declare allocation in current data-environment if data-map was found in this environment
  if (data_map == NULL || data_env != klt_data_environment)
    data_map = iklt_declare_data(data);

  // Case of host device
  if (device_id == 0) {
    if (data->ptr == NULL) {
      assert(0); // TODO maybe
    }
    return;
  }

  // Allocate allocation
  data_map->num_allocations++;
  data_map->allocations = realloc(data_map->allocations, data_map->num_allocations * sizeof(struct iklt_alloc_map_t));
  assert(data_map->allocations != NULL);

  // Initialize allocation
  struct iklt_alloc_map_t * alloc = &(data_map->allocations[data_map->num_allocations - 1]);
  alloc->memloc_id = klt_get_matching_memloc(device_id, data->mode);

  assert(data->sections != NULL);

  size_t i;
  alloc->allocation = malloc(sizeof(struct klt_allocation_t));
  assert(alloc->allocation != NULL);

  alloc->allocation->mode = data->mode;
  alloc->allocation->size = data->base_type_size;
  for (i = 0; i < data->num_sections; i++)
    alloc->allocation->size *= data->sections[i].length;

  struct klt_device_t * device = klt_get_device_by_id(device_id);
  switch (device->kind) {
    case e_klt_host:
    {
      assert(0); // Not reachable
      break;
    }
    case e_klt_threads:
    {
#if KLT_THREADS_ENABLED
      alloc->allocation->descriptor = data->ptr;
      break;
#else /* KLT_THREADS_ENABLED */
      assert(0); // Threads are not enables
#endif /* KLT_THREADS_ENABLED */
    }
    case e_klt_opencl:
    {
#if KLT_OPENCL_ENABLED
      cl_int status;
      alloc->allocation->descriptor = clCreateBuffer(
        device->descriptor.opencl->context,
        CL_MEM_READ_WRITE, // TODO depends on memory location
        alloc->allocation->size,
        NULL,
        &status
      );
//    printf("clCreateBuffer: %x\n", alloc->allocation->descriptor);
      assert(status == CL_SUCCESS);
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

  // Live-in data movement
  if (data->liveness == e_klt_live_in || data->liveness == e_klt_live_inout) {
    switch (device->kind) {
      case e_klt_host:
      {
        assert(0); // Not reachable
        break;
      }
      case e_klt_threads:
      {
#if KLT_THREADS_ENABLED
        // NOP
        break;
#else /* KLT_THREADS_ENABLED */
        assert(0); // Threads are not enables
#endif /* KLT_THREADS_ENABLED */
      }
      case e_klt_opencl:
      {
#if KLT_OPENCL_ENABLED
        cl_int status = clEnqueueWriteBuffer(
          device->descriptor.opencl->queue,
          (cl_mem)alloc->allocation->descriptor,
          CL_TRUE,
          0,
          alloc->allocation->size,
          data->ptr,
          0,
          NULL,
          NULL
        ); /* TODO events chain */
        assert(status == CL_SUCCESS);
        break;
#else /* KLT_OPENCL_ENABLED */
        assert(0); // OpenCL is not enables
#endif /* KLT_OPENCL_ENABLED */
      }
      case e_klt_cuda:
      {
#if KLT_CUDA_ENABLED
        assert(0); // NIY CUDA TODO copy data to the device
#else /* KLT_CUDA_ENABLED */
        assert(0); // CUDA is not enables
#endif /* KLT_CUDA_ENABLED */
      }
    }
  }
}

///

