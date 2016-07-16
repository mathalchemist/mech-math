// =====================================
// Polynom Class 
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

#include "polynom.h"
#include "stdio.h"

int main()
{
  int N = 20;

    double cE[1] = {1.0};
    TPolynom E(0, cE);
    double cB[2] = {-1.0, 1.0};
    TPolynom B(1, cB);

    for (int I = 0; I < N; I++)
    {
      printf("$$");
      (E *= B).Print(); 
      printf("$$");
    }
    for (int I = 0; I < N; I++)
    {
      printf("$$");
      (E /= B).Print(); 
      printf("$$\n");
    }
    printf("\\end");

    return 0;
}
