// DMVN_Lib Implementation

#include "DMVN_Lib.h"

void TPolynom::Resize(int NewDegree)
{
  if (NewDegree > Capacity) // Overflow => enlarge array
  {
    Capacity = NewDegree + RESERVED_SPACE;
    double * NewCoeffs = new double[Capacity + 1];
    memset(NewCoeffs, 0, (Capacity + 1) * sizeof(double));
    for (int I = 0; I <= Degree; I++) NewCoeffs[I] = Coeffs[I];
    delete[] Coeffs;
    Coeffs = NewCoeffs;
  }
  Degree = NewDegree; // set new deg
}

TPolynom TPolynom::operator / (const TPolynom& P) const
{
  TPolynom A(*this); // Divisible
  TPolynom R(0); // Result
  while (A.Degree >= P.Degree)
  {
    TPolynom M(A.Degree - P.Degree); // multiplier
    M.Coeffs[M.Degree] = A.HC() / P.HC();
    A -= (M * P);
    R += M;
  }
  return R;
}

TPolynom& TPolynom::operator /= (const TPolynom& P)
{
  TPolynom R(0); // Result
  while (Degree >= P.Degree)
  {
    TPolynom M(Degree - P.Degree); // multiplier
    M.Coeffs[M.Degree] = HC() / P.HC();
    (*this) -= (M * P);
    R += M;
  }
  return ((*this) = R);
}

TPolynom TPolynom::operator % (const TPolynom& P) const
{
  TPolynom A(*this); // Divisible
  while (A.Degree >= P.Degree)
  {
    TPolynom M(A.Degree - P.Degree); // multiplier
    M.Coeffs[M.Degree] = A.HC() / P.HC();
    A -= (M * P);
  }
  return A;
}

void TPolynom::Print() const
{
  if (!HC()) { printf("0\n"); return; }
  for (int I = Degree; I >= 0; I--)
  {
    if (Coeffs[I] > 0)
    {
      if (I != Degree) printf("+");
    }
    if (fabs(Coeffs[I]) !=  1 || !I) printf("%.0f", Coeffs[I]);
    if (I > 0) printf("x");
    if (I > 1) printf("^%d", I);
  }
  printf("\n");
}

// =================================
// SUBSTITUTION Class implementation
// =================================

TSubst& TSubst::operator *= (const TSubst& S)
{
  // if multiplier is greater, extend self.
  if (N < S.N)
  {
    int * NewArr = new int[S.N];
    for (int i = 0; i < N; i++) NewArr[i] = Arr[i];
    for (int i = N; i < S.N; i++) NewArr[i] = i;
    delete[] Arr;
    Arr = NewArr;
    N = S.N;
  }
  TSubst Temp(*this);
  for (int i = 0; i < S.N; i++) Arr[i] = Temp.Arr[S.Arr[i]];
  return *this;
}

bool TSubst::IsOdd() const
{
  TSubst S(*this);
  
  int Pos = 0;
  int Start = 0;
  int Length = 0;
  while (true)
  {
    Start = Pos;
    int Value = S.Arr[Start]; S.Arr[Start] = -1;
    while (Value != Start)
    {
      int NewValue = S.Arr[Value];
      S.Arr[Value] = -1;
      Value = NewValue;
      Length++;
    } 
    while (Pos < N && S.Arr[Pos] == -1) Pos++;
    if (Pos == S.N) break;
  }
  return (Length & 1);
}

// ===========================
// Matrix class implementation
// ===========================

double TMatrix::Det()
{
  if (W != H) throw Exception("Matrix in not square");
  TMatrix D(*this);
  double det = 1;
  for (int C = 0; C < D.W - 1; C++)
  {
    // Searching maximum abs of element
    int MaxIndex = -1;
    double MaxElement = 0;
    for (int K = C; K < D.H; K++)
    {
      double Element = fabs(D.MATR(C, K));
      if (Element > MaxElement) { MaxIndex = K; MaxElement = Element; }
    }
    if (MaxIndex == -1) return 0;
    // Swapping line [C] and [MaxIndex]
    D.SwapRows(C, MaxIndex);
    if (C != MaxIndex) det = -det;
    // Now [C][C] is nonzero.
    double Norm = D.MATR(C, C);
    for (int j = C + 1; j < D.H; j++) // j-th column
    {
      double Lambda = D.MATR(C, j) / Norm;
      D.SubRows(j, C, Lambda); 
    }
  }
  //D.Print();
  for (int i = 0; i < D.W; i++) det *= D.MATR(i,i);
  return det;
}




