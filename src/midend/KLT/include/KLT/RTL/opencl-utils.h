
#ifndef __KLT_RTL_OPENCL_UTILS_H__
#define __KLT_RTL_OPENCL_UTILS_H__

#if KLT_OPENCL_ENABLED == 0
#  error This file should not be include if KLT is compiled without OpenCL.
#endif

#include <CL/opencl.h>

void klt_opencl_device_to_JSON(cl_platform_id platform, cl_device_id device, const char * indent);

void klt_get_ocl_build_log(cl_device_id device, cl_program program);

////

const char * klt_opencl_status_get_name(cl_int status);

void klt_opencl_assert(cl_int status, const char * opencl_function);

int klt_opencl_check(cl_int status, const char * opencl_function);

#endif /* __KLT_RTL_OPENCL_UTILS_H__ */

