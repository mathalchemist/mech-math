// =====================================
// Matrix Class test
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

#include "matrix.h"

int main()
{
  int N = 0;
  printf("enter size: ");
  scanf("%d", &N);

  TMatrix A(N,N); // The Gilbert Matrix
  int k=0;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
 //     { k++; A[j][i] = k; }
      if (i==j) A[j][i] = 1.0+1.0/double(i+j+1); else A[j][i] = 1.0/double(i+j+1);

//    A.Print();
    printf("Determinant: %1.20lf\n", A.Det());
}
