// =====================================
// Substitution class
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

// =================================
// SUBSTITUTION Class implementation
// =================================

#include "subst.h"

TSubst &TSubst::operator*=(const TSubst &S) {
  // if multiplier is greater, extend self.
  if (N < S.N) {
    int *NewArr = new int[S.N];
    for (int i = 0; i < N; i++)
      NewArr[i] = Arr[i];
    for (int i = N; i < S.N; i++)
      NewArr[i] = i;
    delete[] Arr;
    Arr = NewArr;
    N = S.N;
  }
  TSubst Temp(*this);
  for (int i = 0; i < S.N; i++)
    Arr[i] = Temp.Arr[S.Arr[i]];
  return *this;
}

bool TSubst::IsOdd() const {
  TSubst S(*this);

  int Pos = 0;
  int Start = 0;
  int Length = 0;
  while (true) {
    Start = Pos;
    int Value = S.Arr[Start];
    S.Arr[Start] = -1;
    while (Value != Start) {
      int NewValue = S.Arr[Value];
      S.Arr[Value] = -1;
      Value = NewValue;
      Length++;
    }
    while (Pos < N && S.Arr[Pos] == -1)
      Pos++;
    if (Pos == S.N)
      break;
  }
  return (Length & 1);
}

