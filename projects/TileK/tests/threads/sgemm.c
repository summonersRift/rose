
#include <stdlib.h>
#include <assert.h>

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

extern void sgemm(int nthreads, int n, int m, int p, float alpha, float beta, float ** A, float ** B, float ** C);

int main(int argc, char ** argv) {
  assert(argc == 5);

  int n = atoi(argv[1]);
  int m = atoi(argv[2]);
  int p = atoi(argv[3]);

  int nthreads = atoi(argv[4]);

  float alpha = 1.5;
  float beta = 2.0;

  float ** A = create_array(n, p);
  float ** B = create_array(p, m);
  float ** C = create_array(n, m);

  sgemm(nthreads, n, m, p, alpha, beta, A, B, C);

  free_array(A);
  free_array(B);
  free_array(C);

  return 0;
}

