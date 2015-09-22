
#ifndef __KLT_RTL_DATA_ENVIRONMENT_H__
#define __KLT_RTL_DATA_ENVIRONMENT_H__

#include "KLT/RTL/definition.h"

struct klt_allocation_t;
struct klt_data_t;

/// Create a new data-environment and stack it on top of the existing ones
void klt_push_data_environment();

/// Clear the current data-environment and pop it out of the stack
void klt_pop_data_environment();

/// Return an allocation of 'data' for the device. Search the data-environments recursively
struct klt_allocation_t * klt_get_data(struct klt_data_t * data, size_t device_id);

void klt_allocate_data(struct klt_data_t * data, size_t device_id);
void klt_release_data(struct klt_data_t * data, size_t device_id);

#endif /* __KLT_RTL_DATA_ENVIRONMENT_H__ */

