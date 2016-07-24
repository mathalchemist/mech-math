#include "func.h"

#define A(i, j) a[(i)*n + (j)]

//#define __make_shift__

const real eps = 1e-200;

real RowNorm(real *a, int n) {
  real N = 0;
  for (int i = 0; i < n; i++) {
    real xx = 0;
    for (int j = 0; j < n; j++)
      xx += A(i, j) * A(i, j);
    xx = sqrt(xx);
    if (xx > N)
      N = xx;
  }
  return N;
}

void QRReflect(real *a, int n, int e, real *q) {
  real x0, x1;
  real Nx, Na; // Norm of vector
  real s, dp;  // k-th el. square and dot product

  for (int i = 0; i < e; i++)
    for (int j = 0; j < e; j++)
      q[i * n + j] = (i == j);

  for (int k = 0; k < e - 1; k++) {
    x0 = A(k, k);
    x1 = A(k + 1, k);

    s = x1 * x1;
    Na = sqrt(x0 * x0 + s);
    x0 -= Na;
    Nx = sqrt(x0 * x0 + s);
    if (Nx < eps) {
      printf("QR_Reflect(): Bad norm Nx!\n");
      exit(1);
    }
    Nx = 1.0 / Nx;

    x0 *= Nx;
    x1 *= Nx;

    real dp = 2.0 * (A(k, k + 1) * x0 + A(k + 1, k + 1) * x1);
    A(k, k + 1) -= x0 * dp;
    A(k + 1, k + 1) -= x1 * dp;

    if (k + 2 < e) // range check
    {
      dp = 2.0 * (A(k, k + 2) * x0 + A(k + 1, k + 2) * x1);
      A(k, k + 2) = 0; //-= x0*dp;
      A(k + 1, k + 2) -= x1 * dp;
    }

    A(k, k) = Na;
    A(k + 1, k) = 0.0;

    for (int i = 0; i < e; i++) // row cycle
    {
      dp = 2.0 * (x0 * q[i * n + k] + x1 * q[i * n + k + 1]);
      q[i * n + k] -= x0 * dp;
      q[i * n + k + 1] -= x1 * dp;
    }
  }
}
// fast s/a multiply
void Multiply(real *r, real *q, real *res, int n, int e) {
  for (int i = 0; i < e; i++)
    for (int j = 0; j < e; j++)
      if (i >= j) {
        real s = 0;
        for (int k = 0; k < e; k++)
          s += r[i * n + k] * q[k * n + j];
        res[i * n + j] = s;
        res[j * n + i] = s;
      }
}

// =============================================
void S_Reflect(real acc, real *a, int n, real *q, real *ev) {
  real Row_norm = RowNorm(a, n);
  real *res = new real[n * n];
  q[0] = ev[0] = 0;
  real *x = new real[n]; // vector X
  real s, t, Nx, Na;     // Norm of vector

  for (int k = 0; k < n - 2; k++) // steps. let's begin...
  {
    printf("3-Diag: k = %d\n", k);
    s = 0.0;
    for (int i = k + 2; i < n; i++) {
      x[i] = A(i, k);
      s += x[i] * x[i];
    }
    t = A(k + 1, k);
    Na = sqrt(t * t + s);
    t -= Na;
    x[k + 1] = t;
    Nx = sqrt(s + t * t);
    if (Nx < eps) {
      printf("S_Reflect(): Bad matrix!\n");
      exit(1);
    }
    Nx = 1.0 / Nx;
    for (int i = k + 1; i < n; i++)
      x[i] *= Nx;

    for (int col = k + 1; col < n; col++) // cycle through cols of A
    {
      real dp = 0.0;
      for (int i = k + 1; i < n; i++)
        dp += x[i] * A(i, col);
      dp *= 2.0;
      for (int i = k + 1; i < n; i++)
        A(i, col) -= x[i] * dp;
    }
    A(k + 1, k) = Na;
    for (int i = k + 2; i < n; i++)
      A(i, k) = 0.0;                      // cleanup
    for (int row = k + 1; row < n; row++) // cycle through cols of A
    {
      real dp = 0.0;
      for (int i = k + 1; i < n; i++)
        dp += x[i] * A(row, i);
      dp *= 2.0;
      for (int i = k + 1; i < n; i++)
        A(row, i) -= x[i] * dp;
    }
    A(k, k + 1) = Na;
    for (int j = k + 2; j < n; j++)
      A(k, j) = 0.0; // cleanup
  }
  delete x;
  printf("3-diagonal done.\n");
  for (int k = n; k > 2; k--) // K is current dimension!!!
  {
    printf("QR-step: k = %d\n", k);
    while (fabs(A(k - 1, k - 2)) > acc * Row_norm) {
#ifdef __make_shift__
      s = A(k - 1, k - 1);
      for (int i = 0; i < k; i++)
        A(i, i) -= s;
#endif
      QRReflect(a, n, k, q);
      //	    printf("Matrix R, k = %d:\n", k); PrintMatrix(a, 0, n);
      //	    printf("Matrix Q, k = %d:\n", k); PrintMatrix(q, 0, n);

      Multiply(a, q, res, n, k);

      for (int i = 0; i < n * n; i++)
        a[i] = res[i];
#ifdef __make_shift__
      for (int i = 0; i < k; i++)
        A(i, i) += s;
#endif
      //	    printf("Matrix A:\n"); PrintMatrix(a, 0, n);
    }
  }

  if (n > 1) {
    real t = a[0] + a[n + 1];
    real d = a[0] * a[n + 1] - a[1] * a[n];
    d = t * t - 4 * d;
    if (d < 0)
      printf("suxx: inconsistent equation!!!\n");
    else {
      d = sqrt(d);
      a[0] = (t + d) / 2.0;
      a[n + 1] = (t - d) / 2.0;
    }
  }
  for (int i = 0; i < n; i++)
    ev[i] = A(i, i);
  delete res;
  printf("Row norm: %1.10Lf\n", Row_norm);
}

void PrintMatrix(real *arr, int k, int n) {
  for (int i = k; i < n; i++) {
    for (int j = k; j < n; j++)
      printf("%1.8Lf ", arr[i * n + j]);
    printf("\n");
  }
}

bool ReadMatrix(char *fn, real *array, int m, int n) {
  FILE *F = fopen(fn, "r");
  if (!F)
    return false;
  int c = 0;
  while (!feof(F)) {
    real x;
    int r = fscanf(F, "%Lf", &x);
    if (r != 1) {
      fclose(F);
      return false;
    }
    array[c] = x;
    c++;
  }
  if (c < n * m)
    return false;
  fclose(F);
  return true;
}

real CalcError(real *a, real *ev, int n) {
  real tr = 0;
  real trd = 0;
  for (int i = 0; i < n; i++) {
    tr += a[i * n + i];
    trd += ev[i];
  }
  return fabs(tr - trd);
}
