
#include <stdint.h>
#include <stdlib.h>

#include "utils.c"

void kernel_doitgen(int R, int Q, int P, float *** A, float *** sum, float ** C4) {
  int r, q, p, s;

  #pragma tilek dataenv alloc(  A[0:R][0:Q][0:P], mode:rw, live:inout, device:1) \
                        alloc( C4[0:P][0:P]     , mode:r , live:in   , device:1) \
                        alloc(sum[0:R][0:Q][0:P], mode:rw, live:not  , device:1)
  {
    #pragma tilek kernel device(accelerator, 1) num_gangs[0](4) num_workers[0](4) \
                                                num_gangs[1](4) num_workers[1](4) \
                                                data(  A[0:R][0:Q][0:P], mode:rw, live:inout) \
                                                data( C4[0:P][0:P]     , mode:r , live:in) \
                                                data(sum[0:R][0:Q][0:P], mode:rw, live:not)
    {
      #pragma tilek loop tile[0](dynamic) tile(gang, 0) tile(worker, 0)
      for (r = 0; r < R; r++) {
        #pragma tilek loop tile[1](dynamic) tile(gang, 1) tile(worker, 1)
        for (q = 0; q < Q; q++) {
          #pragma tilek loop tile[0](dynamic)
          for (p = 0; p < P; p++) {
            sum[r][q][p] = 0;
            #pragma tilek loop tile[0](dynamic)
            for (s = 0; s < P; s++)
              sum[r][q][p] = sum[r][q][p] + A[r][q][s] * C4[s][p];
          }
          #pragma tilek loop tile[0](dynamic)
          for (p = 0; p < P; p++)
            A[r][q][p] = sum[r][q][p];
        }
      }
    }
  }
}


int main() {
  size_t R = 128;
  size_t Q = 128;
  size_t P = 128;
  float *** A = tilek_rnd_3D(R, Q, P, 0, 1);
  float *** sum = tilek_rnd_3D(R, Q, P, 0, 0);
  float ** C4 = tilek_rnd_2D(P, P, -1, 1);

  kernel_doitgen(R, Q, P, A, sum, C4);

  free(**A);
  free(*A);
  free(A);

  free(**sum);
  free(*sum);
  free(sum);

  free(*C4);
  free(C4);

  return 0;
}
