
#ifndef ORIGINAL
#define ORIGINAL 0
#endif
#if ORIGINAL == 0
#  include "RTL/Host/tilek-rtl.h"
#  include "KLT/RTL/kernel.h"
#  include "KLT/RTL/loop.h"
#  include "KLT/RTL/tile.h"
#  include "KLT/RTL/data.h"
#endif

#include <stdlib.h>

int main() {
  int i;

  int n = 16;
  float b = 3.5;

  float * A = malloc(n * sizeof(float));
  for (i = 0; i < n; i++)
    A[i] = i;

#if ORIGINAL == 1
  #pragma dataenv alloc(A[0:n], mode:rw, live:io)
//#pragma dataenv alloc(A[0:n], mode:rw, live:io, device:1)
  #pragma kernel data(A[0:n], mode:rw, live:io)
  #pragma loop tile[0](dynamic)
  for (i = 0; i < n; i++)
    A[i] += b;
#else
  klt_push_data_environment();

  {
    struct klt_data_section_t sections[1] = { { 0, n } };
    struct klt_data_t data = { &A[0], 1, sections,  e_klt_read_write, e_klt_live_inout };
    klt_allocate_data(&data, 0);
  //klt_allocate_data(&data, 1);
  }
  
  {
    struct klt_kernel_t * kernel = klt_build_kernel(0);

    kernel->param[0] = &n;
    kernel->param[1] = &b;

    kernel->data[0].ptr = &A[0];
    kernel->data[0].sections[0].offset = 0;
    kernel->data[0].sections[0].length = n;
    kernel->data[0].mode = e_klt_read_write;
    kernel->data[0].liveness = e_klt_live_inout;

    kernel->loops[0].lower = 0;
    kernel->loops[0].upper = n-1;
    kernel->loops[0].stride = 1;

    klt_execute_kernel(kernel);
  }
  klt_pop_data_environment();
#endif

  return 0;
}

