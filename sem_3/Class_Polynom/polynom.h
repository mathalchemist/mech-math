// =====================================
// Polynom Class C
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

#ifndef __POLYNOM_H_INCLUDED__
#define __POLYNOM_H_INCLUDED__

#include <stdio.h>
#include <math.h>
#include <string.h>

inline int Max(int N1, int N2) { if (N1 > N2) return N1; else return N2; }

const int RESERVED_SPACE = 16;

class TPolynom
{
  double * Coeffs;
  int Degree;
  int Capacity;
  TPolynom& RecalcDegree() { while (Degree > 0 && !Coeffs[Degree]) Degree--; return *this; }
  void Resize(int NewDegree);
public:
  class Exception
  {
  public:
    const char * EMsg;
    Exception(): EMsg("") {}
    Exception(const char * ErrorMessage): EMsg(ErrorMessage) {}
  };

  // Default Constructor - empty polynom with zero degree
  TPolynom(): Degree(0), Capacity(RESERVED_SPACE)
  {
    Coeffs = new double[Capacity + 1];
    memset(Coeffs, 0, (Capacity + 1) * sizeof(double));
  }
  // Empty Constructor
  TPolynom(int InitDegree): Degree(InitDegree), Capacity(Degree + RESERVED_SPACE)
  {
    Coeffs = new double[Capacity + 1];
    memset(Coeffs, 0, (Capacity + 1) * sizeof(double));
  }
  // Init Constructor
  TPolynom(int InitDegree, const double * InitCoeffs): Degree(InitDegree), Capacity(Degree + RESERVED_SPACE)
  {
    Coeffs = new double[Capacity + 1];
    memset(Coeffs, 0, (Capacity + 1) * sizeof(double));
    for (int I = 0; I <= Degree; I++) Coeffs[I] = InitCoeffs[I];
  }
  // Copy Constructor
  TPolynom(const TPolynom& P): Degree(P.Degree), Capacity(Degree + RESERVED_SPACE)
  {
    Coeffs = new double[Capacity + 1];
    memset(Coeffs, 0, (Capacity + 1) * sizeof(double));
    for (int I = 0; I <= Degree; I++) Coeffs[I] = P.Coeffs[I];
  }
  // Destructor
  ~TPolynom()
  {
    delete[] Coeffs;
  }

  double& operator [] (int I) // Read/write access
  {
    double Z = 0;
    double& rZ = Z;
    if (I < 0) return rZ;
    if (I > Degree) return rZ;
    return Coeffs[I];
  }
  const double operator [] (int I) const // read-only access
  {
    if (I < 0) return 0.0;
    if (I > Degree) return 0.0;
    return Coeffs[I];
  }
  // Assignment
  TPolynom& operator = (const TPolynom& P)
  {
    Resize(P.Degree);
    for (int I = 0; I <= Degree; I++) Coeffs[I] = P.Coeffs[I];
    return *this;
  }
  // Self - addition
  TPolynom& operator += (const TPolynom& P)
  {
    int NewDegree = Max(Degree, P.Degree);
    Resize(NewDegree);
    for (int I = 0; I <= P.Degree; I++) Coeffs[I] += P.Coeffs[I]; // This code shouldn't gluck because
                                    // P.Degree isn't greater than Degree after Resize().
    RecalcDegree();
    return *this;
  }
  // Self - subtraction
  TPolynom& operator -= (const TPolynom& P)
  {
    int NewDegree = Max(Degree, P.Degree);
    Resize(NewDegree);
    for (int I = 0; I <= P.Degree; I++) Coeffs[I] -= P.Coeffs[I]; // This code shouldn't gluck because
                                    // P.Degree isn't greater than Degree after Resize().
    RecalcDegree();
    return *this;
  }
  // Addition
  TPolynom operator + (const TPolynom& P) const
  {
    int NewDegree = Max(Degree, P.Degree);
    TPolynom R(NewDegree);
    for (int I = 0; I <= NewDegree; I++)
    {
      if (I <= Degree) R.Coeffs[I] = Coeffs[I];
      if (I <= P.Degree) R.Coeffs[I] += P.Coeffs[I];
    }
    return R.RecalcDegree();
  }
  // Subtraction
  TPolynom operator - (const TPolynom& P) const
  {
    int NewDegree = Max(Degree, P.Degree);
    TPolynom R(NewDegree);
    for (int I = 0; I <= NewDegree; I++)
    {
      if (I <= Degree) R.Coeffs[I] = Coeffs[I];
      if (I <= P.Degree) R.Coeffs[I] -= P.Coeffs[I];
    }
    return R.RecalcDegree();
  }
  // Self - multiplication
  TPolynom& operator *= (const TPolynom& P)
  {
    TPolynom R(Degree + P.Degree);
    for (int I = 0; I <= Degree; I++)
      for (int J = 0; J <= P.Degree; J++) R.Coeffs[I + J] += Coeffs[I] * P.Coeffs[J];
    return (*this = R.RecalcDegree());
  }
  TPolynom& operator *= (double Value)
  {
    for (int I = 0; I <= Degree; I++) Coeffs[I] *= Value;
    RecalcDegree();
    return *this;
  }
  TPolynom operator * (const TPolynom P) const
  {
    TPolynom R(Degree + P.Degree);
    for (int I = 0; I <= Degree; I++)
      for (int J = 0; J <= P.Degree; J++) R.Coeffs[I + J] += Coeffs[I] * P.Coeffs[J];
    return R.RecalcDegree();
  }
  TPolynom operator * (double Value) const
  {
    TPolynom R(*this);
    for (int I = 0; I <= Degree; I++) R.Coeffs[I] /= Value;
    return R.RecalcDegree();
  }
  // Division
  TPolynom& operator /= (double Value)
  {
    if (!Value) throw Exception("TPolynom::operator /= (Value): Division by zero");
    for (int I = 0; I <= Degree; I++) Coeffs[I] *= Value;
    return *this;
  }
  TPolynom operator / (double Value) const
  {
    if (!Value) throw Exception("TPolynom::operator /= (Value): Division by zero");
    TPolynom R(*this);
    for (int I = 0; I <= Degree; I++) R.Coeffs[I] /= Value;
    return R;
  }
  TPolynom operator / (const TPolynom& P) const;
  TPolynom operator % (const TPolynom& P) const;
  TPolynom& operator /= (const TPolynom& P);

  double HC() const { return Coeffs[Degree]; } // Highest coefficient
  double LC() const { return Coeffs[0]; } // Lowest coefficient
  int Deg() const { return Degree; }  // Degree

  void Print() const;
};

#endif
