
#include <time.h>
#include <stdlib.h>
#include <assert.h>

void tilek_alloc_init() __attribute__((constructor));

void tilek_alloc_init() {
  srand(time(NULL));
}

float * tilek_alloc_1D(size_t n) {
  return malloc(n * sizeof(float));
}

float ** tilek_alloc_2D(size_t n, size_t m) {
  float ** res = malloc(n * sizeof(float *));
  float * data = malloc(n * m * sizeof(float));
  size_t i;
  for (i = 0; i < n; i++)
    res[i] = &(data[i * m]);
  return res;
}

float *** tilek_alloc_3D(size_t n, size_t m, size_t p) {
  float *** res = malloc(n * sizeof(float **));
  float ** res_ = malloc(n * m * sizeof(float *));
  float * data = malloc(n * m * p * sizeof(float));
  size_t i, j;
  for (i = 0; i < n; i++) {
    res[i] = &(res_[i * m]);
    for (j = 0; j < m; j++)
      res[i][j] = &(data[i * m * p + j * p]);
  }
  return res;
}

float tilek_rnd(float a, float b) {
  if (a == b)
    return a;

  assert(a < b);

  float rnd = (float)rand();
  float norm = (float)(RAND_MAX/(b-a));

  return rnd/norm + a;
}

float * tilek_rnd_1D(size_t n, float a, float b) {
  float * res = tilek_alloc_1D(n);

  size_t i;
  for (i = 0; i < n; i++)
    res[i] = tilek_rnd(a, b);

  return res;
}

float ** tilek_rnd_2D(size_t n, size_t m, float a, float b) {
  float ** res = tilek_alloc_2D(n, m);

  size_t i, j;
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      res[i][j] = tilek_rnd(a, b);

  return res;
}

float *** tilek_rnd_3D(size_t n, size_t m, size_t p, float a, float b) {
  float *** res = tilek_alloc_3D(n, m, p);

  size_t i, j, k;
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      for (k = 0; k < p; k++)
        res[i][j][k] = tilek_rnd(a, b);

  return res;
}


