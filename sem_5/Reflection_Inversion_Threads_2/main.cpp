#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "func.h"

double f(int i, int j) {
  if (i == j)
    return 1.0 + 1.0 / double(i + j + 1);
  else
    return 1.0 / double(i + j + 1.0);
}

void FillMatrix(double *a, int n) {
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      a[i * n + j] = f(i, j);
}

int main() {
  int n, TC;
  // char fn[256];
  printf("Input dimension (n): ");
  scanf("%d", &n);
  if (n <= 0)
    return -1;
  printf("Input number of the threads: ");
  scanf("%d", &TC);
  if (n <= 0)
    return -2;
  // printf("Input source file name (or 'func' to use function): ");
  // scanf("%255s", fn);
  double *a = new double[n * n];
  double *ac = new double[n * n];
  double *ai = new double[n * n];
  printf("Filling...\n");
  /* if (fn[0] == 'f') */ FillMatrix(a, n); /* else ReadMatrix(fn, a, n, n); */
  printf("Saving...\n");
  for (int i = 0; i < n * n; i++)
    ac[i] = a[i];
  printf("Inverting...\n");
  timeval tv1;
  gettimeofday(&tv1, 0); // get current time
  if (!S_Reflect(TC, a, n, ai)) {
    printf("Bad matrix!\n");
    return -3;
  }
  timeval tv2;
  gettimeofday(&tv2, 0); // again get current time

  // Get time in microseconds, then divide by 1.000.000
  double Time = (double(tv2.tv_sec) * 1000000.0 + double(tv2.tv_usec) -
                 double(tv1.tv_sec) * 1000000.0 + double(tv1.tv_usec)) /
                1000000.0;

  printf("Result:\n");
  PrintMatrix(ai, 0, n);
  printf("\n\nThreads: %d,\nError: %1.17lf,\nTime=%1.4lf sec.\n", TC,
         CalcError(ac, ai, n), Time);
  delete a;
  delete ac;
  delete ai;
  return 0;
}
