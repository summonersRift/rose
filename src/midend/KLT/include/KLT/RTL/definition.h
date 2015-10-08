
#ifndef __KLT_RTL_DEFINITION_H__
#define __KLT_RTL_DEFINITION_H__

#ifndef KLT_KERNEL_TIMING
#define KLT_KERNEL_TIMING 0
#endif

#ifndef KLT_SUBKERNEL_TIMING
#define KLT_SUBKERNEL_TIMING 0
#endif

#ifndef KLT_THREADS_ENABLED
#define KLT_THREADS_ENABLED 0
#endif

#ifndef KLT_OPENCL_ENABLED
#define KLT_OPENCL_ENABLED 0
#endif

#ifndef KLT_CUDA_ENABLED
#define KLT_CUDA_ENABLED 0
#endif

#include <stddef.h>

enum klt_tile_kind_e {
  e_klt_tile_static  = 0,
  e_klt_tile_dynamic = 1,
  e_klt_tile_thread  = 2,
  e_klt_tile_gang    = 3,
  e_klt_tile_worker  = 4
};

enum klt_memory_mode_e {
  e_klt_mode_unknown = 0,
  e_klt_read_only = 1,
  e_klt_write_only = 2,
  e_klt_read_write = 3
};

enum klt_liveness_e {
  e_klt_live_unknown = 0,
  e_klt_live_in = 1,
  e_klt_live_out = 2,
  e_klt_live_inout = 3,
  e_klt_live_not = 4
};

enum klt_device_e {
  e_klt_host = 1,
  e_klt_threads = 2,
  e_klt_opencl = 3,
  e_klt_cuda = 4
};

struct klt_loop_context_t;
struct klt_data_context_t;

typedef void (*klt_host_kernel_func_ptr)(void **, void **, struct klt_loop_context_t *, struct klt_data_context_t *);

#if KLT_THREADS_ENABLED
typedef void (*klt_threads_kernel_func_ptr)(int, void **, void **, struct klt_loop_context_t *, struct klt_data_context_t *);
#endif

#endif /* __KLT_RTL_DEFINITION_H__ */

