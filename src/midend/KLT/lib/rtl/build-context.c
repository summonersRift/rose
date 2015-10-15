
#include "KLT/RTL/kernel.h"
#include "KLT/RTL/context.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/build-context.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

int iklt_get_tile_length(struct klt_kernel_t * kernel, enum klt_tile_kind_e kind, unsigned long param);
int iklt_get_tile_length(struct klt_kernel_t * kernel, enum klt_tile_kind_e kind, unsigned long param) {
  switch (kind) {
    case e_klt_tile_static:
      return param;
    case e_klt_tile_dynamic:
      assert(0);
      return 0;
    case e_klt_tile_thread:
      return kernel->num_threads;
    case e_klt_tile_gang:
      return kernel->num_gangs[param];
    case e_klt_tile_worker:
      return kernel->num_workers[param];
    default:
      assert(0);
      return 0;
  }
}

void klt_solve_one_loop(struct klt_loop_desc_t * loop_desc, struct klt_loop_context_t * loop_ctx, struct klt_kernel_t * kernel, int loop_it);
void klt_solve_one_loop(struct klt_loop_desc_t * loop_desc, struct klt_loop_context_t * loop_ctx, struct klt_kernel_t * kernel, int loop_it) {
  int loop_idx = loop_desc[loop_it].idx;

  int length = klt_get_loop_upper(loop_ctx, loop_idx) - klt_get_loop_lower(loop_ctx, loop_idx) + 1;
  int stride = klt_get_loop_stride(loop_ctx, loop_idx);

//printf("klt_solve_one_loop :   loop_it = %d, loop_idx = %d, num_tiles = %d\n", loop_it, loop_idx, loop_desc[loop_it].num_tiles);
//printf("                       loop_ctx->num_loops = %d\n", loop_ctx->num_loops);
//dump_loop_ctx(loop_ctx);

  int tile_it = 0;
  while (tile_it < loop_desc[loop_it].num_tiles && loop_desc[loop_it].tile_desc[tile_it].kind != e_klt_tile_dynamic) {
    int tile_idx = loop_desc[loop_it].tile_desc[tile_it].idx;

//  printf("klt_solve_one_loop : > tile_it = %d, tile_idx = %d, kind = %d\n", tile_it, tile_idx, loop_desc[loop_it].tile_desc[tile_it].kind);

    klt_set_tile_length(loop_ctx, tile_idx, length);
    length /= iklt_get_tile_length(kernel, loop_desc[loop_it].tile_desc[tile_it].kind, loop_desc[loop_it].tile_desc[tile_it].param);
    klt_set_tile_stride(loop_ctx, tile_idx, length);
    tile_it++;
  }

//printf("klt_solve_one_loop :   HALF-WAY DOWN (tile_it = %d)\n", tile_it);
//dump_loop_ctx(loop_ctx);

  int dyn_tile = tile_it;
  tile_it = loop_desc[loop_it].num_tiles - 1;
  while (tile_it >= 0 && loop_desc[loop_it].tile_desc[tile_it].kind != e_klt_tile_dynamic) {
    int tile_idx = loop_desc[loop_it].tile_desc[tile_it].idx;

//  printf("klt_solve_one_loop : < tile_it = %d, tile_idx = %d, kind = %d\n", tile_it, tile_idx, loop_desc[loop_it].tile_desc[tile_it].kind);

    klt_set_tile_stride(loop_ctx, tile_idx, stride);
    stride *= iklt_get_tile_length(kernel, loop_desc[loop_it].tile_desc[tile_it].kind, loop_desc[loop_it].tile_desc[tile_it].param);
    klt_set_tile_length(loop_ctx, tile_idx, stride);
    tile_it--;
  }

//printf("klt_solve_one_loop :   HALF-WAY UP (tile_it = %d)\n", tile_it);
//dump_loop_ctx(loop_ctx);

  assert((tile_it == dyn_tile && loop_desc[loop_it].tile_desc[tile_it].kind == e_klt_tile_dynamic) || (tile_it == 0 && dyn_tile == loop_desc[loop_it].num_tiles));
  if (loop_desc[loop_it].tile_desc[tile_it].kind == e_klt_tile_dynamic) {
    int tile_idx = loop_desc[loop_it].tile_desc[tile_it].idx;
    klt_set_tile_length(loop_ctx, tile_idx, length);
    klt_set_tile_stride(loop_ctx, tile_idx, stride);
  }

//printf("klt_solve_one_loop :   DONE\n");
//dump_loop_ctx(loop_ctx);
}

void klt_solve_loop_context(int num_loops, struct klt_loop_desc_t * loop_desc, struct klt_loop_context_t * loop_ctx, struct klt_kernel_t * kernel);
void klt_solve_loop_context(int num_loops, struct klt_loop_desc_t * loop_desc, struct klt_loop_context_t * loop_ctx, struct klt_kernel_t * kernel) {
  int loop_it;

  for (loop_it = 0; loop_it < num_loops; loop_it++)
    if (loop_desc[loop_it].num_tiles > 0)
      klt_solve_one_loop(loop_desc, loop_ctx, kernel, loop_it);
}

void dump_loop_ctx(struct klt_loop_context_t * loop_ctx);
void dump_loop_ctx(struct klt_loop_context_t * loop_ctx) {
  int i;
  int * ptr = loop_ctx->data;
  for (i = 0; i < loop_ctx->num_loops; i++) {
    printf("Loop #%d : %d to %d by %d\n", i, *(ptr+0), *(ptr+1), *(ptr+2));
    ptr += 3;
  }
  for (i = 0; i < loop_ctx->num_tiles; i++) {
    printf("Tile #%d : %d by %d\n", i, *(ptr+0), *(ptr+1));
    ptr += 2;
  }
}

struct klt_loop_context_t * klt_build_loop_context(struct klt_loop_container_t * loop_container, struct klt_loop_t * loops_, struct klt_kernel_t * kernel) {
  struct klt_loop_t * loops = malloc(loop_container->num_loops * sizeof(struct klt_loop_t));
  int i;
  for (i = 0; i < loop_container->num_loops; i++)
    memcpy(&(loops[i]), &(loops_[loop_container->loop_desc[i].idx]), sizeof(struct klt_loop_t));
//  loops[i] = loops_[loop_container->loop_desc[i].idx];

//printf("klt_build_loop_context :   loop_container->num_loops = %d\n", loop_container->num_loops);
//printf("klt_build_loop_context :   loop_container->num_tiles = %d\n", loop_container->num_tiles);

  size_t size = sizeof(struct klt_loop_context_t) + loop_container->num_loops * 3 * sizeof(int) + loop_container->num_tiles * 2 * sizeof(int);

//printf("klt_build_loop_context :   size(loop_context) = %d\n", size);

  struct klt_loop_context_t * loop_ctx = malloc(size);

  memset(loop_ctx, 0, size);

  loop_ctx->num_loops = loop_container->num_loops;
  loop_ctx->num_tiles = loop_container->num_tiles;
  memcpy(loop_ctx->data, loops, loop_container->num_loops * 3 * sizeof(int));

//dump_loop_ctx(loop_ctx);

  klt_solve_loop_context(loop_container->num_loops, loop_container->loop_desc, loop_ctx, kernel);

//dump_loop_ctx(loop_ctx);

  return loop_ctx;
}

struct klt_loop_context_t * klt_copy_loop_context(struct klt_loop_context_t * loop_context) {
  size_t size = sizeof(struct klt_loop_context_t) + loop_context->num_loops * 3 * sizeof(int) + loop_context->num_tiles * 2 * sizeof(int);
  struct klt_loop_context_t * res = malloc(size);
  memcpy(res, loop_context, size);
  return res;
}

struct klt_data_context_t * klt_build_data_context(size_t num_data) {
  struct klt_data_context_t * res = malloc(sizeof(struct klt_data_context_t));
  res->num_data = num_data;
  return res;
}

struct klt_data_context_t * klt_copy_data_context(struct klt_data_context_t * data_context) {
  struct klt_data_context_t * res = malloc(sizeof(struct klt_data_context_t));
  memcpy(res, data_context, sizeof(struct klt_data_context_t));
  return res;
}

