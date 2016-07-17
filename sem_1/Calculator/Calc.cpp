// Integer arithmetic with very big numbers (up to 10^MAX_LENGTH)
// (C) SEGMENTATION FAULT Software Inc, 2003

#include "Calc.h"

#define MAX_LENGTH 10000

TNum::TNum(void) {
  Data = (char *)malloc(MAX_LENGTH);
  memset(Data, 0, MAX_LENGTH);
}

TNum::TNum(int N) {
  Data = (char *)malloc(MAX_LENGTH);
  *this = N;
}

TNum::TNum(TNum &N) {
  Data = (char *)malloc(MAX_LENGTH);
  *this = N;
}

TNum::~TNum(void) {
  if (Data)
    free(Data);
}

void TNum::Write(char *FileName) {
  FILE *F = fopen(FileName, "w+");
  for (int I = GetLength() - 1; I >= 0; I--)
    fprintf(F, "%d", Data[I]);
  if (!GetLength())
    fprintf(F, "%d", 0);
  fclose(F);
}

void TNum::Display(void) {
  for (int I = GetLength() - 1; I >= 0; I--)
    printf("%d", Data[I]);
  if (!GetLength())
    printf("%d", 0);
}

int TNum::GetLength(void) {
  int Length;
  for (int I = MAX_LENGTH - 1; I >= 0; I--)
    if (Data[I])
      return I + 1;
  return 1;
}

TNum &TNum::operator=(int N) {
  memset(Data, 0, MAX_LENGTH);
  int I = 0;
  while (N) {
    Data[I] = N % 10;
    N /= 10;
    I++;
  }
  return *this;
}

TNum &TNum::operator=(TNum &N) {
  int L = N.GetLength();
  for (int I = 0; I < L; I++)
    Data[I] = N.Data[I];
  return *this;
}

bool TNum::operator==(TNum &N) {
  for (int I = 0; I < MAX_LENGTH; I++)
    if (Data[I] != N.Data[I])
      return false;
  return true;
}

bool TNum::operator==(int N) {
  TNum Num(N);
  return *this == Num;
}

bool TNum::operator!=(TNum &N) { return !(*this == N); }

bool TNum::operator!=(int N) {
  TNum Num(N);
  return !(*this == Num);
}

TNum &TNum::operator++(int) {
  *this += 1;
  return *this;
}

TNum &TNum::operator++(void) {
  *this += 1;
  return *this;
}

TNum &TNum::operator--(int) {
  for (int I = 0; I < MAX_LENGTH; I++)
    if (Data[I]) {
      Data[I]--;
      return *this;
    } else
      Data[I] = 9;
  // possible wrong result
  return *this;
}

TNum &TNum::operator--(void) {
  (*this)--;
  return *this;
}

TNum &TNum::operator+=(TNum &N) {
  char C = 0; // Carry flag
  for (int I = 0; I < MAX_LENGTH; I++) {
    char S = Data[I] + N.Data[I] + C;
    if (S > 9) {
      C = 1;
      S = S % 10;
    } else
      C = 0;
    Data[I] = S;
  }
  return *this;
}

TNum &TNum::operator+=(int N) {
  TNum Num(N);
  *this += Num;
  return *this;
}

TNum &TNum::operator-=(TNum &N) {
  if (N != 0)
    for (TNum I(0); I != N; I++)
      (*this)--;
  return *this;
}

TNum &TNum::operator-=(int N) {
  TNum Num(N);
  *this += Num;
  return *this;
}

TNum &TNum::operator*=(TNum &N) {
  TNum Result(0);
  int L = N.GetLength();
  if (N != 0)
    for (int I = 0; I < L; I++) // cycle through multiplier digits
    {
      TNum A(0);
      char C = 0;
      for (int J = 0; J < MAX_LENGTH; J++) {
        char P = Data[J] * N.Data[I] + C;
        if (P > 9) {
          C = P / 10; // Carry
          P = P % 10; // Digit
        } else
          C = 0;
        if (I + J < MAX_LENGTH)
          A.Data[I + J] = P;
      }
      Result += A;
    }
  *this = Result;
  return *this;
}

TNum &TNum::operator*=(int N) {
  TNum Num(N);
  *this *= Num;
  return *this;
}
