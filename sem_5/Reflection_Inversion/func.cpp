#include "func.h"

#define A(i, j) a[(i)*n + (j)]
#define AI(i, j) ai[(i)*n + (j)]

//#define __step_debug__

const double eps = 1e-30;

bool S_Reflect(double *a, int n, double *ai) {
  double *x = new double[n]; // vector X
  double s, t, Nx, Na;       // Norm of vector

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      ai[i * n + j] = (i == j);

  for (int k = 0; k < n - 1; k++) // steps. let's begin...
  {
#ifdef __step_debug__
    printf("Reflect-step: %d\n", k);
#endif
    s = 0.0;
    for (int i = k + 1; i < n; i++) {
      x[i] = A(i, k);
      s += x[i] * x[i];
    }
    t = A(k, k);
    Na = sqrt(t * t + s); // norm of vector a_1
    t -= Na;
    x[k] = t; // x[k] = a_1 - |a_1|*e_1

    Nx = sqrt(t * t + s);
    if (Nx < eps)
      return false;
    Nx = 1.0 / Nx;
    for (int i = k; i < n; i++)
      x[i] *= Nx;

    for (int col = k + 1; col < n; col++) // cycle through cols of A
    {
      // calc dot prod. of x and y
      double dp = 0.0;
      for (int i = k; i < n; i++)
        dp += x[i] * A(i, col);
      for (int i = k; i < n; i++)
        A(i, col) -= 2.0 * x[i] * dp;
    }
    A(k, k) = Na;

    for (int col = 0; col < n; col++) {
      double dp = 0.0;
      for (int i = k; i < n; i++)
        dp += x[i] * ai[i * n + col];
      for (int i = k; i < n; i++)
        ai[i * n + col] -= 2.0 * x[i] * dp;
    }
  }
  delete x;

  for (int k = 0; k < n; k++) {
#ifdef __step_debug__
    printf("Gauss reverse step: %d\n", k);
#endif
    GaussReverse(a, ai, k, n);
  }
  return true;
}

void GaussReverse(double *a, double *ai, int col, int n) {
  for (int k = 1; k <= n; k++) {
    double koeff = AI(n - k, col);
    for (int i = n - k + 1; i < n; i++)
      koeff -= AI(i, col) * A(n - k, i);
    AI(n - k, col) = koeff / A(n - k, n - k);
  }
}

void PrintMatrix(double *arr, int k, int n) {
  for (int i = k; i < n; i++) {
    for (int j = k; j < n; j++)
      printf("%1.5lf ", arr[i * n + j]);
    printf("\n");
  }
}

bool ReadMatrix(char *fn, double *array, int m, int n) {
  FILE *F = fopen(fn, "r");
  if (!F)
    return false;
  int c = 0;
  while (!feof(F)) {
    double x;
    int r = fscanf(F, "%lf", &x);
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

double CalcError(double *a, double *ai, int n) {
  double er = 0.0;
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      double s = 0.0;
      for (int k = 0; k < n; k++)
        s += a[i * n + k] * ai[k * n + j];
      if (i == j)
        er += fabs(s - 1.0);
      else
        er += fabs(s);
    }
  return er;
}
