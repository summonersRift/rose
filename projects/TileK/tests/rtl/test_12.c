
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

float ** create_array(int n, int m) {
  float ** a = malloc(n * sizeof(float *));
  float * a_ = malloc(n * m * sizeof(float));

  int i, j;

  for (i = 0; i < n; i++) {
    a[i] = a_ + i * m;
    for (j = 0; j < m; j++) {
      a[i][j] = i+j;
    }
  }

  return a;
}

void free_array(float ** a) {
  free(a[0]);
  free(a);
}

void kernel_0(int n, int m, float ** A, float b) {
  int i, j;
}

int main() {
  int i;

  int n = 16;
  int m = 16;
  int p = 16;
  int q = 16;

  float alpha = 3.5;
  float beta = 3.5;

  float ** A = create_array(n, p);
  float ** B = create_array(p, q);
  float ** C = create_array(q, m);

  float ** D = create_array(n, q);
  float ** E = create_array(n, m);

#if ORIGINAL == 1
  #pragma dataenv alloc(A[0:n][0:p], mode:r, live:in) \
                  alloc(B[0:p][0:q], mode:r, live:in) \
                  alloc(C[0:q][0:m], mode:r, live:in) \
                  alloc(D[0:n][0:q], mode:rw, live:not) \
                  alloc(E[0:n][0:m], mode:rw, live:out)
  #pragma tilek kernel data(A[0:n][0:p], mode:r, live:in) \
                       data(B[0:p][0:q], mode:r, live:in) \
                       data(C[0:q][0:m], mode:r, live:in) \
                       data(D[0:n][0:q], mode:rw, live:not) \
                       data(E[0:n][0:m], mode:rw, live:out)
  {
    #pragma tilek loop tile[0](dynamic)
    for (i = 0; i < n; i++) {
      #pragma tilek loop tile[1](dynamic)
      for (j = 0; j < q; j++) {
        D[i][j] = 0;
        for (k = 0; k < p; k++) {
          D[i][j] += A[i][k] * B[k][j];
        }
        D[i][j] *= alpha;
      }
    } // n, p, q, alpha // A, B, D
    #pragma tilek loop tile[0](dynamic)
    for (i = 0; i < n; i++) {
      #pragma tilek loop tile[1](dynamic)
      for (j = 0; j < m; j++) {
        E[i][j] = 0;
        for (k = 0; k < q; k++) {
          E[i][j] += D[i][k] * C[k][j];
        }
        E[i][j] *= beta;
      }
    } // n, m, q, beta // C, D, E
  }
#else
  klt_push_data_environment();

  {
    struct klt_data_section_t sections[2] = { { 0, n }, { 0, p } };
    struct klt_data_t data = { &A[0][0], 2, sections,  e_klt_read_only, e_klt_live_in };
    klt_allocate_data(&data, 0);
  }
  {
    struct klt_data_section_t sections[2] = { { 0, p }, { 0, q } };
    struct klt_data_t data = { &B[0][0], 2, sections,  e_klt_read_only, e_klt_live_in };
    klt_allocate_data(&data, 0);
  }
  {
    struct klt_data_section_t sections[2] = { { 0, q }, { 0, m } };
    struct klt_data_t data = { &C[0][0], 2, sections,  e_klt_read_only, e_klt_live_in };
    klt_allocate_data(&data, 0);
  }
  {
    struct klt_data_section_t sections[2] = { { 0, n }, { 0, q } };
    struct klt_data_t data = { &D[0][0], 2, sections,  e_klt_read_write, e_klt_live_not };
    klt_allocate_data(&data, 0);
  }
  {
    struct klt_data_section_t sections[2] = { { 0, n }, { 0, m } };
    struct klt_data_t data = { &E[0][0], 2, sections,  e_klt_read_write, e_klt_live_out };
    klt_allocate_data(&data, 0);
  }

  {
    struct klt_kernel_t * kernel = klt_build_kernel(0);

    kernel->param[0] = &n;
    kernel->param[1] = &m;
    kernel->param[2] = &p;
    kernel->param[3] = &q;
    kernel->param[4] = &alpha;
    kernel->param[5] = &beta;

    kernel->data[0].ptr = &A[0][0];
    kernel->data[0].sections[0].offset = 0;
    kernel->data[0].sections[0].length = n;
    kernel->data[0].sections[1].offset = 0;
    kernel->data[0].sections[1].length = p;
    kernel->data[0].mode = e_klt_read_only;
    kernel->data[0].liveness = e_klt_live_in;

    kernel->data[1].ptr = &B[0][0];
    kernel->data[1].sections[0].offset = 0;
    kernel->data[1].sections[0].length = p;
    kernel->data[1].sections[1].offset = 0;
    kernel->data[1].sections[1].length = q;
    kernel->data[1].mode = e_klt_read_only;
    kernel->data[1].liveness = e_klt_live_in;

    kernel->data[2].ptr = &C[0][0];
    kernel->data[2].sections[0].offset = 0;
    kernel->data[2].sections[0].length = q;
    kernel->data[2].sections[1].offset = 0;
    kernel->data[2].sections[1].length = m;
    kernel->data[2].mode = e_klt_read_only;
    kernel->data[2].liveness = e_klt_live_in;

    kernel->data[3].ptr = &D[0][0];
    kernel->data[3].sections[0].offset = 0;
    kernel->data[3].sections[0].length = n;
    kernel->data[3].sections[1].offset = 0;
    kernel->data[3].sections[1].length = q;
    kernel->data[3].mode = e_klt_read_write;
    kernel->data[3].liveness = e_klt_live_not;

    kernel->data[4].ptr = &E[0][0];
    kernel->data[4].sections[0].offset = 0;
    kernel->data[4].sections[0].length = n;
    kernel->data[4].sections[1].offset = 0;
    kernel->data[4].sections[1].length = m;
    kernel->data[4].mode = e_klt_read_write;
    kernel->data[4].liveness = e_klt_live_out;

    kernel->loops[0].lower = 0;
    kernel->loops[0].upper = n-1;
    kernel->loops[0].stride = 1;

    kernel->loops[1].lower = 0;
    kernel->loops[1].upper = q-1;
    kernel->loops[1].stride = 1;

    kernel->loops[2].lower = 0;
    kernel->loops[2].upper = n-1;
    kernel->loops[2].stride = 1;

    kernel->loops[3].lower = 0;
    kernel->loops[3].upper = m-1;
    kernel->loops[3].stride = 1;

    klt_execute_kernel(kernel);
  }

  klt_pop_data_environment();
#endif

  return 0;
}

