
#include <stdlib.h>

#include "utils.c"

int main() {
  int i;

  int n = 16;
  float b = 3.5;

  float * v = tilek_rnd_1D(n, 0, 1);

  #pragma tilek dataenv alloc(v[0:n], device:0, mode:rw, live:inout)
  #pragma tilek kernel device(host, 0) data(v[0:n], mode:rw, live:inout)
  #pragma tilek loop tile[0](dynamic)
  for (i = 0; i < n; i++)
    v[i]+=b;

  free(v);

  return 0;
}

