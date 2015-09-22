
#pragma OPENCL EXTENSION cl_khr_fp64: enable

#include "KLT/RTL/context.h"

__kernel void kernel_k0_v0_0(int n, float b, __global float * v, __constant struct klt_loop_context_t * loop_ctx, __constant struct klt_data_context_t * data_ctx) {
  int t_0;
  int t_1 = get_group_id(0) * klt_get_tile_stride(loop_ctx, 1);
  int t_2 = get_local_id(0) * klt_get_tile_stride(loop_ctx, 2);
  int l_0;
  for (t_0 = 0; t_0 < klt_get_tile_length(loop_ctx, 0); t_0 += klt_get_tile_stride(loop_ctx, 0)) {
    l_0 = klt_get_loop_lower(loop_ctx, 0) + t_0 + t_1 + t_2;
    v[l_0] += b;
  }
}

