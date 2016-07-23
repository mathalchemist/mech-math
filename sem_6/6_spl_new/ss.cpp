#include "ss.h"

void PrintMatrix(double *a, double *b, int nn) {
  for (int i = 0; i < nn; i++) {
    for (int j = 0; j < nn; j++)
      printf("%1.4lf ", Ass(i, j));
    printf(" | %1.4lf\n", b[i]);
  }
}
// print solution X
void PrintX(double *x, int nn) {
  for (int i = 0; i < nn; i++)
    printf("%1.7lf ", x[i]);
}

// main solving function
bool SolveSystem(int nn, double *a, double *b, double *x) {
  //    PrintMatrix(a,b,nn);
  for (int k = 0; k < nn; k++) // steps
  {
    double max = 0;
    int MR = k;
    for (int i = k + 1; i < nn; i++)
      if (fabs(Ass(i, k)) > max) {
        MR = i;
        max = fabs(Ass(i, k));
      }

    if (MR != k) {
      for (int j = k; j < nn; j++) {
        double tmp = Ass(k, j);
        Ass(k, j) = Ass(MR, j);
        Ass(MR, j) = tmp;
      }
      double tmpb = b[k];
      b[k] = b[MR];
      b[MR] = tmpb;
    }
    double p = Ass(k, k);

    if (fabs(p) < 1e-200)
      return false;

    p = 1.0 / p;
    for (int j = k; j < nn; j++)
      Ass(k, j) *= p;
    b[k] *= p;

    for (int i = k + 1; i < nn; i++) {
      p = Ass(i, k);
      for (int j = k; j < nn; j++)
        Ass(i, j) -= p * Ass(k, j);
      b[i] -= p * b[k];
    }
  }
  // Gauss's "reverse step"
  for (int i = nn - 1; i >= 0; i--) {
    double p = b[i];
    for (int j = i + 1; j < nn; j++)
      p -= x[j] * Ass(i, j);
    x[i] = p / Ass(i, i);
  }
  return true;
}
