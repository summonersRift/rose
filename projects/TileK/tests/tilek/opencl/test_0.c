
#include "KLT/RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/data.h"

#include <stdlib.h>

#include "utils.c"

int main() {
  int i;

  int n = 256 * 1024 * 1024;
  float b = 3.5;

  float * v = tilek_rnd_1D(n, 0, 1);

  #pragma tilek dataenv alloc(v[0:n], device:1, mode:rw, live:inout)
  #pragma tilek kernel device(acc, 1) num_gangs[0](4) num_workers[0](4) data(v[0:n], mode:rw, live:inout)
  #pragma tilek loop tile[0](dynamic) tile(gang, 0) tile(worker, 0)
  for (i = 0; i < n; i++)
    v[i]+=b;

  free(v);

  return 0;
}
