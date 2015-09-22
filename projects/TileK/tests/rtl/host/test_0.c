
#include "KLT/RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/data.h"

#include <stdlib.h>

int main() {
  int i;

  int n = 16;
  float b = 3.5;

  float * v = malloc(n * sizeof(float));
  for (i = 0; i < n; i++)
    v[i] = i;

  {
    klt_push_data_environment();

    struct klt_data_section_t sections_0[1] = {
      { 0, n }
    };

    struct klt_data_t data[1] = {
      { &(v[0]), 1, sections_0,  e_klt_read_write, e_klt_live_inout }
    };

    klt_allocate_data(&(data[0]), 0);
  
    {
      struct klt_kernel_t * kernel = klt_build_kernel(0);

      kernel->param[0] = &n;
      kernel->param[1] = &b;

      kernel->data[0].ptr = &(v[0]);
      kernel->data[0].sections[0].offset = 0;
      kernel->data[0].sections[0].length = n;
      kernel->data[0].mode = e_klt_read_write;
      kernel->data[0].liveness = e_klt_live_inout;

      kernel->loops[0].lower = 0;
      kernel->loops[0].upper = n-1;
      kernel->loops[0].stride = 1;

      kernel->device_id = 0; // last to be set, it might depend on parameters, data, and loops
      
      klt_execute_kernel(kernel);
    }

    klt_pop_data_environment();
  }

  return 0;
}

