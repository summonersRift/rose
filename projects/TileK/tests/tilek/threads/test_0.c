
#include "KLT/RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/data.h"

#include <stdlib.h>

int main() {
  int i;

  int n = 256 * 1024 * 1024;
  float b = 3.5;

  float * v = malloc(n * sizeof(float));
  for (i = 0; i < n; i++)
    v[i] = i;

  #pragma tilek dataenv alloc(v[0:n], device:1, mode:rw, live:inout)
  #pragma tilek kernel device(threads, 1) num_threads(8) data(v[0:n], mode:rw, live:inout)
  #pragma tilek loop tile[0](dynamic) tile(thread)
  for (i = 0; i < n; i++)
    v[i]+=b;

  return 0;
}

