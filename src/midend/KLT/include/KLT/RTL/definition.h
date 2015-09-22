
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
  e_klt_read_only,
  e_klt_write_only,
  e_klt_read_write
};

enum klt_liveness_e {
  e_klt_live_unknown = 0,
  e_klt_live_in,
  e_klt_live_out,
  e_klt_live_inout,
  e_klt_live_not
};

enum klt_device_e {
  e_klt_host = 0,
  e_klt_threads,
  e_klt_opencl,
  e_klt_cuda
};

struct klt_loop_context_t;
struct klt_data_context_t;

typedef void (*klt_host_kernel_func_ptr)(void **, void **, struct klt_loop_context_t *, struct klt_data_context_t *);

#if KLT_THREADS_ENABLED
typedef void (*klt_threads_kernel_func_ptr)(int, void **, void **, struct klt_loop_context_t *, struct klt_data_context_t *);
#endif

#endif /* __KLT_RTL_DEFINITION_H__ */

