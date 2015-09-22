
#include <stdio.h>
#include <stdlib.h>

float * create_vector(int n) {
  float * v = malloc(n * sizeof(float));

  int i;

  for (i = 0; i < n; i++)
    v[i] = i;

  return v;
}

void print_vector(int n_0, int n_1, float * v) {
  int i;
  for (i = n_0; i < n_1; i++) {
    printf(" | %f", v[i]);
  }
  printf(" |\n\n");
}

void kernel_0(int n, float * v, float b) {
  int i;
  #pragma tilek dataenv alloc(v[0:n], mode:rw, live:inout, device:0)
  #pragma tilek kernel data(v[0:n], mode:rw, live:inout)
  #pragma tilek loop tile[0](dynamic)
  for (i = 0; i < n; i++)
    v[i]+=b;
}

int main() {

  int n = 16;

  float * v;

  {
    v = create_vector(n);

    print_vector(0, 10, v);

    kernel_0(n, v, 3.5);

    print_vector(0, 10, v);

    free(v);
  }

  return 0;
}

