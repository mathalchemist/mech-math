// =====================================
// Substitution class
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

#ifndef __SUBST_H_INCLUDED__
#define __SUBST_H_INCLUDED__

#include <stdio.h>

class TSubst {
  int *Arr;
  int N;

public:
  TSubst(int *arr, int n) : Arr(0), N(n) {
    Arr = new int[N];
    for (int i = 0; i < N; i++)
      Arr[i] = arr[i] % N;
  }
  TSubst(const TSubst &S) : Arr(0), N(S.N) {
    Arr = new int[N];
    for (int i = 0; i < N; i++)
      Arr[i] = S.Arr[i] % N;
  }
  ~TSubst() { delete[] Arr; }
  TSubst &operator=(const TSubst &S) {
    if (N < S.N) {
      delete[] Arr;
      Arr = new int[S.N];
    }
    N = S.N;
    for (int i = 0; i < N; i++)
      Arr[i] = S.Arr[i] % N;
    return *this;
  }
  TSubst &operator*=(const TSubst &S);
  TSubst operator*(const TSubst &S) {
    TSubst R(*this);
    R *= S;
    return R;
  }
  int At(int X) const { return Arr[X % N]; }
  bool IsOdd() const;
  void Print() const {
    for (int i = 0; i < N; i++)
      printf("===");
    printf("\n");
    for (int i = 0; i < N; i++)
      printf("%3d", i);
    printf("\n");
    for (int i = 0; i < N; i++)
      printf("%3d", Arr[i]);
    printf("\n");
  }
};

#endif
