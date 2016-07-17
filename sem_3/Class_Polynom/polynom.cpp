// =====================================
// Polynom Class
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

#include "polynom.h"

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
  if (!HC()) { printf("0"); return; }
  for (int I = Degree; I >= 0; I--)
  {
    if (Coeffs[I] > 0)
    {
      if (I != Degree) printf("+");
    }
    if (fabs(Coeffs[I]) !=  1 || !I) printf("%.0f", Coeffs[I]);
    if (I > 0) printf("x");
    if (I > 1) printf("^{%d}", I);
  }
}

