
#include "KLT/RTL/kernel.h"

char * opencl_kernel_file = NULL;
char * opencl_kernel_options = NULL;
char * opencl_klt_runtime_lib = NULL;

char * klt_file_stem = "";
size_t klt_num_kernels = 0;
struct klt_kernel_desc_t klt_kernel_desc[0];

