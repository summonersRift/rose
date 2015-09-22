
#ifndef __KLT_RTL_INIT_H__
#define __KLT_RTL_INIT_H__

#include "KLT/RTL/definition.h"

void klt_init() __attribute__((constructor));

#if KLT_OPENCL_ENABLED
const char * ocl_status_to_char(int status);
#endif /* KLT_OPENCL_ENABLED */

#endif /* __KLT_RTL_INIT_H__ */

