
#include <stdint.h>
#include <stdlib.h>

void kernel_doitgen(size_t R, size_t Q, size_t P, float *** A, float *** sum, float ** C4) {
  int r, q, p, s;

  #pragma tilek dataenv alloc(  A[0:R][0:Q][0:P], mode:rw, live:inout, device:0) \
                        alloc( C4[0:P][0:P]     , mode:rw, live:in   , device:0) \
                        alloc(sum[0:R][0:Q][0:P], mode:rw, live:not  , device:0)
  {
    #pragma tilek kernel device(host, 0) data(  A[0:R][0:Q][0:P], mode:rw, live:inout) \
                                         data( C4[0:P][0:P]     , mode:rw, live:in) \
                                         data(sum[0:R][0:Q][0:P], mode:rw, live:not)
    {
      #pragma tilek loop tile[0](dynamic)
      for (r = 0; r < R; r++) {
        #pragma tilek loop tile[1](dynamic)
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
  return 0;
}
