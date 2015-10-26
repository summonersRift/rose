
#include "KLT/RTL/definition.h"

#include "KLT/RTL/device.h"
#include "KLT/RTL/memory.h"
#include "KLT/RTL/data.h"
#include "KLT/RTL/kernel.h"

#include <unistd.h>

#include <stdio.h>

#include <stdlib.h>

#include <assert.h>

size_t klt_devices_count = 0;
struct klt_device_t * klt_devices = NULL;

size_t iklt_increase_alloc_devices(void) {
  size_t res = klt_devices_count;
  klt_devices_count += 1;
  klt_devices = realloc(klt_devices, klt_devices_count * sizeof(struct klt_device_t));
  assert(klt_devices != NULL);
  return res;
}

size_t iklt_device_increase_alloc_subdevices(struct klt_device_t * device) {
  size_t res = device->num_subdevices;
  device->num_subdevices += 1;
  device->subdevices = realloc(device->subdevices, device->num_subdevices * sizeof(struct klt_device_t *));
  assert(device->subdevices != NULL);
  return res;
}

size_t iklt_device_increase_alloc_memlocs(struct klt_device_t * device) {
  size_t res = device->num_memlocs;
  device->num_memlocs += 1;
  device->memlocs = realloc(device->memlocs, device->num_memlocs * sizeof(struct klt_memloc_t));
  assert(device->memlocs != NULL);
  return res;
}

struct klt_memloc_t * klt_get_matching_memloc(size_t device_id, enum klt_memory_mode_e mode) {
  assert(mode != e_klt_mode_unknown);
  assert(klt_devices != NULL && device_id < klt_devices_count);

  struct klt_device_t * device = klt_get_device_by_id(device_id);
  assert(device != NULL);
  assert(device->memlocs != NULL);

  size_t i;
  for (i = 0; i < device->num_memlocs; i++)
    if (device->memlocs[i].mode == mode)
      return &(device->memlocs[i]);

  for (i = 0; i < device->num_memlocs; i++)
    if (device->memlocs[i].mode == e_klt_read_write)
      return &(device->memlocs[i]);

  return NULL;
}

struct klt_device_t * klt_get_device_by_id(size_t id) {
  assert(id < klt_devices_count);
  return &(klt_devices[id]);
}

#if KLT_THREADS_ENABLED
void * iklt_threads_worker(void * arg);
void * iklt_threads_worker(void * arg) {
  struct klt_threads_t * thread = (struct klt_threads_t *)arg;

  usleep(10);

//printf("Thread #%d waits for mutex (%x) ...\n", thread->tid, &(thread->work_mutex));

  pthread_mutex_lock(&(thread->work_mutex));

  while (1) {
    pthread_cond_wait(&(thread->work_cond), &(thread->work_mutex));
    if (thread->finished == 1) break;

//  printf("Thread #%d started working.\n", thread->tid);

    assert(thread->workload != NULL);
    assert(thread->workload->kernel_func != NULL);

    (*thread->workload->kernel_func)(thread->tid, thread->workload->parameters, thread->workload->data, thread->workload->loop_context, thread->workload->data_context);

//  printf("Thread #%d done working.\n", thread->tid);

    free(thread->workload->parameters);
    free(thread->workload->data);
    free(thread->workload->loop_context);
    free(thread->workload->data_context);
    free(thread->workload);
    thread->workload = NULL;

    pthread_mutex_lock(&(thread->done_mutex));
    pthread_cond_signal(&(thread->done_cond));
    pthread_mutex_unlock(&(thread->done_mutex));
  }
  pthread_mutex_unlock(&(thread->work_mutex));

  pthread_exit(NULL);
}

void klt_threads_submit_workload(struct klt_threads_device_t * device, size_t tid, struct klt_threads_workload_t * workload) {
  assert(tid < device->num_threads);

  pthread_mutex_lock(&(device->threads[tid].work_mutex));
  assert(device->threads[tid].workload == NULL);
  device->threads[tid].workload = workload;
  pthread_cond_signal(&(device->threads[tid].work_cond));
  pthread_mutex_unlock(&(device->threads[tid].work_mutex));

}

void klt_threads_wait_for_completion(struct klt_threads_device_t * device) {
  size_t i;
  for (i = 0; i < device->num_threads; i++) {
//  printf("Waiting for thread #%d (mutex %x)\n", i, &(device->threads[i].done_mutex));
    pthread_mutex_lock(&(device->threads[i].done_mutex));
    if (device->threads[i].workload != NULL) {
      pthread_cond_wait(&(device->threads[i].done_cond), &(device->threads[i].done_mutex));
    }
    pthread_mutex_unlock(&(device->threads[i].done_mutex));
  }
}

struct klt_threads_device_t * iklt_build_threads_device(size_t num_threads) {
  struct klt_threads_device_t * res = malloc(sizeof(struct klt_threads_device_t));
    res->num_threads = num_threads;
    res->threads = malloc(num_threads * sizeof(struct klt_threads_t));

  pthread_attr_t threads_attr;
  pthread_attr_init(&threads_attr);
  pthread_attr_setdetachstate(&threads_attr, PTHREAD_CREATE_JOINABLE);

  int rc;

  size_t i;
  for (i = 0; i < num_threads; i++) {
    res->threads[i].tid = i;

    pthread_mutex_init(&(res->threads[i].work_mutex), NULL);
    pthread_cond_init(&(res->threads[i].work_cond), NULL);

    pthread_mutex_init(&(res->threads[i].done_mutex), NULL);
    pthread_cond_init(&(res->threads[i].done_cond), NULL);

    res->threads[i].workload = NULL;

    res->threads[i].finished = 0;

    usleep(100);

    rc = pthread_create(&(res->threads[i].pthread), &threads_attr, iklt_threads_worker, &(res->threads[i]));
    assert(!rc);

    usleep(100);
  }

  pthread_attr_destroy(&threads_attr);

  return res;
}

void iklt_destroy_threads_device(struct klt_threads_device_t * device);
void iklt_destroy_threads_device(struct klt_threads_device_t * device) {
  klt_threads_wait_for_completion(device);

  size_t i;
  for (i = 0; i < device->num_threads; i++) {
    pthread_mutex_lock(&(device->threads[i].work_mutex));
    assert(device->threads[i].workload == NULL);
    device->threads[i].finished = 1;
    pthread_cond_signal(&(device->threads[i].work_cond));
    pthread_mutex_unlock(&(device->threads[i].work_mutex));
  }

  void * status;
  int rc;
  for (i = 0; i < device->num_threads; i++) {    
    rc = pthread_join(device->threads[i].pthread, &status);
    assert(!rc);
  }
}
#endif /* KLT_THREADS_ENABLED */

#if KLT_OPENCL_ENABLED
struct klt_opencl_device_t * klt_build_opencl_device(cl_platform_id platform, cl_device_id device, size_t num_sources, const char ** sources, char * options) {
  cl_int status;
  struct klt_opencl_device_t * res = malloc(sizeof(struct klt_opencl_device_t));

  res->platform = platform;
  res->device = device;

  res->context = clCreateContext(0, 1, &(res->device), NULL, NULL, &status);
  klt_opencl_assert(status, "clCreateContext");

  res->queue = clCreateCommandQueue(res->context, device, 0, &status);
  klt_opencl_assert(status, "clCreateCommandQueue");

/*{
    printf("*******************************************\n");
    size_t i;
    printf("// %d input sources\n", num_sources);
    for (i = 0; i < num_sources; i++) {
      printf("// sources[%zd]:\n", i);
      printf("%s\n", sources[i]);
    }
    printf("*******************************************\n");
  }*/

  if (num_sources > 1) {
    res->program = clCreateProgramWithSource(res->context, num_sources, sources, NULL, &status);
    klt_opencl_assert(status, "clCreateProgramWithSource");

    status = clBuildProgram(res->program, 1, &(res->device), options, NULL, NULL);
    if (status == CL_BUILD_PROGRAM_FAILURE) {
      klt_get_ocl_build_log(res->device, res->program);
    }
    klt_opencl_assert(status, "clBuildProgram");
  }
  else {
    res->program = NULL;
#if VERBOSE
    printf("[Warning] No OpenCL sources found. The device's program was not initialized!\n");
#endif
  }

  return res;
}
#endif /* KLT_OPENCL_ENABLED */

void klt_dbg_dump_device(size_t id) {
  struct klt_device_t * device = klt_get_device_by_id(id);
  printf("  {");
  printf( "\n    \"id\":%zd", device->device_id);
  printf(",\n    \"kind\":%u", device->kind);
  switch (device->kind) {
    case e_klt_host: break; /* NOP */
#if KLT_THREADS_ENABLED
    case e_klt_threads:
    {
      printf(",\n    \"num_threads\":%zd", device->descriptor.threads->num_threads);
      break;
    }
#endif
#if KLT_OPENCL_ENABLED
    case e_klt_opencl:
    {
      printf(",\n    \"opencl\":");
      klt_opencl_device_to_JSON(device->descriptor.opencl->platform, device->descriptor.opencl->device, "    ");
      break;
    }
#endif
#if KLT_CUDA_ENABLED
    case e_klt_cuda:
    {
      assert(0); // TODO struct klt_cuda_device_t * cuda;
      break;
    }
#endif
    default:
      assert(0);
  }
  printf("\n  }");
}

void klt_dbg_dump_all_devices(void) {
  size_t i;
  printf("[\n");
  for (i = 0; i < klt_devices_count; i++) {
    if (i > 0) printf(",\n");
    klt_dbg_dump_device(i);
  }
  printf("\n]\n");
}

