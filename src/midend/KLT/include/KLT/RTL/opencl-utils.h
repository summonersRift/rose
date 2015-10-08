
#ifndef __KLT_RTL_OPENCL_UTILS_H__
#define __KLT_RTL_OPENCL_UTILS_H__

#if KLT_OPENCL_ENABLED == 0
#  error This file should not be include if KLT is compiled without OpenCL.
#endif

#include <CL/opencl.h>

char * klt_read_file(const char * filename);

void klt_get_ocl_build_log(cl_device_id device, cl_program program);

void klt_print_opencl_status(cl_int status);

void klt_check_opencl_status(char * msg, cl_int status);

#endif /* __KLT_RTL_OPENCL_UTILS_H__ */

