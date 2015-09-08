
#include "KLT/RTL/data-environment.h"
#include "KLT/RTL/device.h"
#include "KLT/RTL/memory.h"
#include "KLT/RTL/data.h"

#include <stdlib.h>

#include <assert.h>

struct iklt_alloc_map_t {
  struct klt_memloc_t * memloc;
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

struct klt_data_environment_t * klt_build_data_environment(struct klt_data_environment_t * parent) {
  struct klt_data_environment_t * res = malloc(sizeof(struct klt_data_environment_t));

  res->parent = parent;
  res->num_data = 0;
  res->allocations = NULL;

  return res;
}

void klt_clear_data_environment(struct klt_data_environment_t * data_env) {
  size_t i, j;
  for (i = 0; i < data_env->num_data; i++) {
    for (j = 0; j < data_env->allocations[i].num_allocations; j++) {
      if (data_env->allocations[i].allocations[j].memloc->memloc_id != 0)
        klt_user_free_data_on_device(data_env->allocations[i].data, data_env->allocations[i].allocations[j].memloc, data_env->allocations[i].allocations[j].allocation);
      free(data_env->allocations[i].allocations[j].allocation);
      data_env->allocations[i].allocations[j].memloc = NULL;
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

void klt_push_data_environment() {
  klt_data_environment = klt_build_data_environment(klt_data_environment);
}

void klt_pop_data_environment() {
  struct klt_data_environment_t * tmp = klt_data_environment;
  klt_data_environment = klt_data_environment->parent;
  klt_clear_data_environment(tmp);
}

///

struct iklt_data_map_t * iklt_get_data_map(struct klt_data_environment_t * data_env, struct klt_data_t * data) {
  size_t i;
  for (i = 0; i < data_env->num_data; i++)
    if (klt_same_data(data_env->allocations[i].data, data) == 0)
      return &(data_env->allocations[i]);
  return NULL;
}

struct iklt_data_map_t * iklt_lookup_data(struct klt_data_t * data) {
  struct klt_data_environment_t * data_env = klt_data_environment;
  while (data_env != NULL) {
    struct iklt_data_map_t * data_map = iklt_get_data_map(data_env, data);
    if (data_map != NULL) return data_map;
    data_env = data_env->parent;
  }
  return NULL;
}

struct klt_allocation_t * iklt_search_alloc_map(struct klt_data_environment_t * data_env, struct klt_data_t * data, size_t memloc_id) {
  struct iklt_data_map_t * data_map = iklt_get_data_map(data_env, data);
  if (data_map == NULL)
    return NULL;

  size_t i;
  for (i = 0; i < data_map->num_allocations; i++)
    if (data_map->allocations[i].memloc->memloc_id == memloc_id)
      return data_map->allocations[i].allocation;
  return NULL;
}

struct klt_allocation_t * klt_get_data(struct klt_data_t * data, size_t device_id) {
  struct iklt_data_map_t * data_map = iklt_lookup_data(data);
  if (data == NULL) return NULL;

  struct klt_memloc_t * memloc = klt_get_matching_memloc(device_id, data->mode);

  size_t i;
  for (i = 0; i < data_map->num_allocations; i++)
    if (data_map->allocations[i].memloc->memloc_id == memloc->memloc_id)
      return data_map->allocations[i].allocation;

  return NULL;
}

///

struct iklt_data_map_t * klt_declare_data(struct klt_data_t * data) {
  klt_data_environment->num_data++;
  klt_data_environment->allocations = realloc(klt_data_environment->allocations, klt_data_environment->num_data * sizeof(struct iklt_data_map_t));
  assert(klt_data_environment->allocations != NULL);

  struct iklt_data_map_t * data_map = &(klt_data_environment->allocations[klt_data_environment->num_data - 1]);
    data_map->data = data;
    data_map->num_allocations = 1;
    data_map->allocations = malloc(sizeof(struct iklt_alloc_map_t));
    data_map->allocations[0].memloc = klt_get_matching_memloc(0, e_klt_read_write);
    data_map->allocations[0].allocation = malloc(sizeof(struct klt_allocation_t));
    data_map->allocations[0].allocation->mode = data->mode;
    data_map->allocations[0].allocation->size = 0;
    data_map->allocations[0].allocation->user_descriptor = data->ptr;
  return data_map;
}

void klt_allocate_data(struct klt_data_t * data, size_t device_id) {
  struct iklt_data_map_t * data_map = iklt_lookup_data(data);
  if (data_map == NULL)
    data_map = klt_declare_data(data);

  if (device_id == 0) {
    if (data->ptr == NULL) {
      assert(0); // TODO maybe
    }
    return;
  }

  data_map->num_allocations++;
  data_map->allocations = realloc(data_map->allocations, data_map->num_allocations * sizeof(struct iklt_alloc_map_t));
  assert(data_map->allocations != NULL);

  struct iklt_alloc_map_t * alloc = &(data_map->allocations[data_map->num_allocations - 1]);
    alloc->memloc = klt_get_matching_memloc(device_id, data->mode);
    alloc->allocation = klt_user_allocate_data_on_device(data, alloc->memloc);
}

///

