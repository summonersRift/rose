
#ifndef __KLT_RTL_SOLVE_LOOP_CONTEXT_H__
#define __KLT_RTL_SOLVE_LOOP_CONTEXT_H__

#include "KLT/RTL/definition.h"

struct klt_kernel_t;
struct klt_loop_container_t;
struct klt_loop_t;

struct klt_loop_context_t * klt_build_loop_context(struct klt_loop_container_t *, struct klt_loop_t *, struct klt_kernel_t *);

struct klt_loop_context_t * klt_copy_loop_context(struct klt_loop_context_t *);

struct klt_data_context_t * klt_build_data_context(size_t num_data);

struct klt_data_context_t * klt_copy_data_context(struct klt_data_context_t *);

#endif /* __KLT_RTL_SOLVE_LOOP_CONTEXT_H__ */

