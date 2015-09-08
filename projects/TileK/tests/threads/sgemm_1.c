
void sgemm(int nthreads, int n, int m, int p, float alpha, float beta, float ** A, float ** B, float ** C) {
  int i, j, k;

  #pragma tilek kernel num_threads(nthreads) \
                       data(A[0:n][0:p], mode:r, live:in) \
                       data(B[0:p][0:m], mode:r, live:in) \
                       data(C[0:n][0:m], mode:rw, live:inout)
  {
    #pragma tilek loop tile(thread) tile[0](dynamic)
    for (i = 0; i < n; i++)
      #pragma tilek loop tile[1](dynamic)
      for (j = 0; j < m; j++) {
        C[i][j] *= beta;
        for (k = 0; k < p; k++)
          C[i][j] += alpha * A[i][k] * B[k][j];
      }
  }
}

