// ==========================
// DMVN Class Library
// DMVN_Lib.h
// (C) DMVN Corporation, 2003
// Last updated: [07.12.03]
// ==========================
// Imp |   Name   | Debug
// ======================
//  +  | TVector  |   +
//  +  | TPolynom |   +
//  -  | TMatrix  |   -
//  +  | TSubst   |   +

#ifndef __DMVN_LIB_H_INCLUDED__
#define __DMVN_LIB_H_INCLUDED__

#include "math.h"
#include "string.h"
#include "stdio.h"

// =========================
// 3D Vector class interface
// =========================

class TVector
{
public:
  double X, Y, Z;
  // constructors
  TVector(): X(0), Y(0), Z(0) {}
  TVector(const TVector& V): X(V.X), Y(V.Y), Z(V.Z) {}
  TVector(double x, double y, double z): X(x), Y(y), Z(z) {}
  // Destructor
  ~TVector() {};
  // operators
    TVector& operator = (const TVector& V) { X = V.X; Y = V.Y; Z = V.Z; return *this; }
  TVector operator + (const TVector& V) const { return TVector(X + V.X, Y + V.Y, Z + V.Z); }
  TVector operator - (const TVector& V) const { return TVector(X - V.X, Y - V.Y, Z - V.Z); }
  TVector& operator += (const TVector& V) { X += V.X; Y += V.Y; Z += V.Z; return *this; }
  TVector& operator -= (const TVector& V) { X -= V.X; Y -= V.Y; Z -= V.Z; return *this; }
  TVector& operator *= (double Value) { X *= Value; Y *= Value; Z *= Value; return *this; }
  TVector operator * (double Value) const { return TVector(X * Value, Y * Value, Z * Value); }
  // Scalar product
  double operator | (const TVector& V) const { return X * V.X, Y * V.Y, Z * V.Z; }
  // Cross product
  TVector operator & (const TVector& V) const { return TVector(Y * V.Z - Z * V.Y, Z * V.X - X * V.Z, X * V.Y - Y * V.X); }
  bool operator == (const TVector& V) const { return (X == V.X && Y == V.Y && Z == V.Z); }
  bool operator != (const TVector& V) const { return !operator ==(V); }
  // methods
  double Length(void) const { return sqrt(X*X + Y*Y + Z*Z); }
  TVector& Normalize(void) { double L = Length(); if (!L) return *this; X /= L; Y /= L; Z /= L; return *this; }

    double Angle(const TVector& V) const { double L = Length() * V.Length(); if (!L) return 0; return acos(((*this) | V) / L); }
};

inline TVector operator * (double Value, const TVector& V) { return TVector(Value * V.X, Value * V.Y, Value * V.Z); }

// ==========================
// Polynomial class interface
// ==========================

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

// ============================
// SUBSTITUTION Class interface
// ============================

class TSubst
{
  int * Arr;
  int N;
public:
  TSubst(int * arr, int n): Arr(0), N(n)
  {
    Arr = new int[N]; for (int i = 0; i < N; i++) Arr[i] = arr[i] % N;
  }
  TSubst(const TSubst& S): Arr(0), N(S.N)
  {
    Arr = new int[N]; for (int i = 0; i < N; i++) Arr[i] = S.Arr[i] % N;
  }
  ~TSubst() { delete[] Arr; }
  TSubst& operator = (const TSubst& S)
  {
    if (N < S.N) { delete[] Arr; Arr = new int[S.N]; } N = S.N;
    for (int i = 0; i < N; i++) Arr[i] = S.Arr[i] % N; return *this;
  }
  TSubst& operator *= (const TSubst& S);
  TSubst operator * (const TSubst& S) { TSubst R(*this); R *= S; return R; }
  int At(int X) const { return Arr[X % N]; }
  bool IsOdd() const;
  void Print() const
  {
    for (int i = 0; i < N; i++) printf("==="); printf("\n");
    for (int i = 0; i < N; i++) printf("%3d", i); printf("\n");
    for (int i = 0; i < N; i++) printf("%3d", Arr[i]); printf("\n");
  }
};

// MATRIX Class interface

#define MATR(i,j) Arr[j*W+i]
const double EPSILON = 0.00000000001;

class TMatrix
{
  double * Arr;
  int W; // width
  int H; // height
public:
  class Exception
  {
  public:
    const char * EMsg;
    Exception(): EMsg("") {}
    Exception(const char * ErrorMessage): EMsg(ErrorMessage) {}
  };

  TMatrix(int Width, int Height): Arr(0), W(Width), H(Height) // init with zeros
  {
    Arr = new double[W*H];
    for (int j = 0; j < H; j++) for (int i = 0; i < W; i++) MATR(i,j) = 0;
  }
  TMatrix(int Width, int Height, double * arr): Arr(0), W(Width), H(Height) // init with data
  {
    Arr = new double[W*H];
    for (int j = 0; j < H; j++) for (int i = 0; i < W; i++) MATR(i,j) = arr[j*W+i];
  }
  TMatrix(const TMatrix& M): Arr(0), W(M.W), H(M.H)
  {
    Arr = new double[W*H];
    for (int i = 0; i < W; i++) for (int j = 0; j < H; j++) MATR(i,j) = M.Arr[j*W+i];
  }
  TMatrix& operator = (const TMatrix& M)
  {
    if (M.W != W || M.H != H)
    {
      W = M.W; H = M.H; delete[] Arr;
      Arr = new double[W*H];
    }
    for (int j = 0; j < H; j++) for (int i = 0; i < W; i++) MATR(i,j) = M.MATR(i,j);
    return *this;
  }
  double * operator [] (int j) const { return Arr+W*j; }
  TMatrix& operator *= (double Value)
  {
    for (int j = 0; j < H; j++) for (int i = 0; i < W; i++) MATR(i,j) *= Value;
    return *this;
  }
  TMatrix& operator / (double Value)
  {
    if (fabs(Value) < EPSILON) throw Exception("Division by zero");
    for (int j = 0; j < H; j++) for (int i = 0; i < W; i++) MATR(i,j) /= Value;
    return *this;
  }
  TMatrix operator * (const TMatrix& M)
  {
    if (W != M.H) throw Exception("Cannot multiply matrices"); // width of M1 = height of M2
    TMatrix R(M.W, H);
    for (int j = 0; j < R.H; j++) for (int i = 0; i < R.W; i++)
        for (int k = 0; k < W; k++) R.MATR(i,j) += MATR(k,j) * M.MATR(i,k);
    return R;
  }
  TMatrix& operator *= (const TMatrix& M)
  {
    if (W != M.H) throw Exception("Cannot multiply matrices"); // width of M1 = height of M2
    TMatrix R(M.W, H);
    for (int j = 0; j < R.H; j++) for (int i = 0; i < R.W; i++)
        for (int k = 0; k < W; k++) R.MATR(i,j) += MATR(k,j) * M.MATR(i,k);
    return (*this = R);
  }
  void SwapRows(int M, int N)
  {
    if (M == N) return;
    for (int i = 0; i < W; i++) { double Temp = MATR(i, M); MATR(i, M) = MATR(i, N); MATR(i, N) = Temp; }
  }
  void SwapCols(int M, int N)
  {
    if (M == N) return;
    for (int j = 0; j < H; j++) { double Temp = MATR(M, j); MATR(M, j) = MATR(N, j); MATR(N, j) = Temp; }
  }
  void SubRows(int M, int N, double Lambda) // M-th row -= (N-th * Lambda)
  {
    for (int k = 0; k < W; k++) MATR(k, M) -= MATR(k, N) * Lambda;
  }
  double Det(); // Determinant
  void Print() // Console matrix dump
  {
    for (int j = 0; j < H; j++) { for (int i = 0; i < W; i++) printf("%1.7lf ", MATR(i,j)); printf("\n"); }
    printf("\n");
  }
};


#endif
