
#include <stdint.h>
#include <stdlib.h>

#include "utils.c"

void kernel_syr2k(size_t n, size_t m, float alpha, float beta, float ** A, float ** B, float ** C) {
  int i, j, k;

  #pragma tilek dataenv alloc(A[0:n][0:m], mode:rw, live:inout, device:0) \
                        alloc(B[0:n][0:m], mode:r , live:in   , device:0) \
                        alloc(C[0:n][0:n], mode:r , live:in   , device:0)
  {
    #pragma tilek kernel device(host, 0) data(A[0:n][0:m], mode:rw, live:inout) \
                                         data(B[0:n][0:m], mode:r , live:in   ) \
                                         data(C[0:n][0:n], mode:r , live:in   )
    {
      #pragma tilek loop tile[3](static, 4) tile[1](dynamic)
      for (i = 0; i < n; i++) {
        #pragma tilek loop tile[0](dynamic) tile[2](static, 4)
        for (j = 0; j < n; j++) {
          C[i][j] *= beta;
          #pragma tilek loop tile[0](dynamic)
          for (k = 0; k < m; k++) {
            C[i][j] += alpha * A[i][k] * B[j][k];
            C[i][j] += alpha * B[i][k] * A[j][k];
          }
        }
      }
    }
  }
}


int main() {
  size_t n = 16;
  size_t m = 16;
  float alpha = tilek_rnd(-5, 5);
  float beta = tilek_rnd(-5, 5);
  float ** A = tilek_rnd_2D(n, m, -100, 100);
  float ** B = tilek_rnd_2D(n, n, -100, 100);
  float ** C = tilek_rnd_2D(n, n, -100, 100);

  kernel_syr2k(n, m, alpha, beta, A, B, C);

  free(*A);
  free(A);

  free(*B);
  free(B);

  free(*C);
  free(C);

  return 0;
}

