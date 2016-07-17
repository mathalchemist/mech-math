// (C) SEGMENTATION FAULT Software, 2003
// 3D matrix & vector library

#ifndef __DMVN_3DMATHS_H_INCLUDED__
#define __DMVN_3DMATHS_H_INCLUDED__

#include "math.h"

class TMatrix;
class TVector;

// TVector class
class TVector {
public:
  double Coords[4];
  TVector(void);
  TVector(double X, double Y, double Z);
  TVector(const TVector &V);
  void Clear(void);
  void Set(double X, double Y, double Z);
  TVector &operator=(const TVector &V);
  TVector &operator+=(const TVector &V);
  TVector &operator-=(const TVector &V);
  TVector &operator*=(double Value);
  TVector &operator*=(const TMatrix &M);
  TVector &operator/=(double Value);
  bool operator==(const TVector &V) const;
  bool operator!=(const TVector &V) const;
  double Length(void) const;
  TVector &Normalize(void);
  void Add(double X, double Y, double Z);
  void Subtract(double X, double Y, double Z);
  void RotateX(double Angle);
  void RotateY(double Angle);
  void RotateZ(double Angle);
  void Rotate(TVector *Dir, double Angle);
};

// TMatrix class
class TMatrix {
public:
  double Elements[4][4];
  TMatrix(void);             // Empty matrix
  TMatrix(double Value);     // Init main diagonal with 'Value'
  TMatrix(const TMatrix &M); // Init with matrix 'M'
  TMatrix(double E00, double E01, double E02,
          double E03, // Init matrix with array of elements
          double E10, double E11, double E12, double E13, double E20,
          double E21, double E22, double E23, double E30, double E31,
          double E32, double E33);
  void Clear(void); // Clear matrix
  void
  Clear(double Value); // Clear matrix & fill its main diagonal with 'Value'

  TMatrix &operator=(const TMatrix &M);
  TMatrix &operator*=(const TMatrix &M);

  void RotateX(double Angle);
  void RotateY(double Angle);
  void RotateZ(double Angle);
  void Rotate(TVector *Dir, double Angle);
  void MoveTo(double X, double Y, double Z); // Absolute shift
  void MoveTo(TVector *V);                   // Absolute shift
  void Move(double X, double Y, double Z);   // Relative shift
  void Move(TVector *V);                     // Relative shift
};

TVector operator+(const TVector &A, const TVector &B);
TVector operator-(const TVector &A, const TVector &B);
TVector operator*(double Value, const TVector &V);
TVector operator*(const TVector &V, double Value);
TVector operator&(const TVector &A, const TVector &B); // Vector product
double operator|(const TVector &A, const TVector &B);  // Scalar product
TVector operator*(const TVector &V, const TMatrix &M);
TVector operator/(const TVector &V, double Value);

TMatrix operator*(const TMatrix &A, const TMatrix &B);

// Implementation
TVector::TVector(void) { Clear(); }

TVector::TVector(double X, double Y, double Z) { Set(X, Y, Z); }

TVector::TVector(const TVector &V) { *this = V; }

void TVector::Clear(void) { Set(0, 0, 0); }

void TVector::Set(double X, double Y, double Z) {
  Coords[0] = X;
  Coords[1] = Y;
  Coords[2] = Z;
  Coords[3] = 1;
}

TVector &TVector::operator=(const TVector &V) {
  memmove(&Coords, &V.Coords, sizeof(double) * 4);
  return *this;
}

TVector &TVector::operator+=(const TVector &V) {
  Coords[0] += V.Coords[0];
  Coords[1] += V.Coords[1];
  Coords[2] += V.Coords[2];
  return *this;
}

void TVector::Add(double X, double Y, double Z) {
  Coords[0] += X;
  Coords[1] += Y;
  Coords[2] += Z;
}

void TVector::Subtract(double X, double Y, double Z) {
  Coords[0] -= X;
  Coords[1] -= Y;
  Coords[2] -= Z;
}

TVector &TVector::operator-=(const TVector &V) {
  Coords[0] -= V.Coords[0];
  Coords[1] -= V.Coords[1];
  Coords[2] -= V.Coords[2];
  return *this;
}

TVector &TVector::operator*=(double Value) {
  Coords[0] *= Value;
  Coords[1] *= Value;
  Coords[2] *= Value;
  return *this;
}

TVector &TVector::operator*=(const TMatrix &M) {
  int I, J;
  TVector R;

  for (I = 0; I < 4; I++) {
    double Value = 0;
    for (J = 0; J < 4; J++)
      Value += Coords[J] * M.Elements[I][J];
    R.Coords[I] = Value;
  }
  *this = R;
  return *this;
}

TVector &TVector::operator/=(double Value) {
  double InvValue;

  if (Value) {
    InvValue = 1 / Value;
    *this *= InvValue;
  }
  return *this;
}

bool TVector::operator==(const TVector &V) const {
  return Coords[0] == V.Coords[0] && Coords[1] == V.Coords[1] &&
         Coords[2] == V.Coords[2];
}

bool TVector::operator!=(const TVector &V) const {
  return Coords[0] != V.Coords[0] || Coords[1] != V.Coords[1] ||
         Coords[2] != V.Coords[2];
}

double TVector::Length(void) const {
  return (double)sqrt(Coords[0] * Coords[0] + Coords[1] * Coords[1] +
                      Coords[2] * Coords[2]);
}

TVector operator+(const TVector &A, const TVector &B) {
  return TVector(A.Coords[0] + B.Coords[0], A.Coords[1] + B.Coords[1],
                 A.Coords[2] + B.Coords[2]);
}

TVector operator-(const TVector &A, const TVector &B) {
  return TVector(A.Coords[0] - B.Coords[0], A.Coords[1] - B.Coords[1],
                 A.Coords[2] - B.Coords[2]);
}

TVector operator*(double Value, const TVector &V) {
  return TVector(V.Coords[0] * Value, V.Coords[1] * Value, V.Coords[2] * Value);
}

TVector operator*(const TVector &V, double Value) {
  return TVector(V.Coords[0] * Value, V.Coords[1] * Value, V.Coords[2] * Value);
}

TVector operator&(const TVector &A, const TVector &B) {
  return TVector(A.Coords[1] * B.Coords[2] - A.Coords[2] * B.Coords[1],
                 A.Coords[2] * B.Coords[0] - A.Coords[0] * B.Coords[2],
                 A.Coords[0] * B.Coords[1] - A.Coords[1] * B.Coords[0]);
}

double operator|(const TVector &A, const TVector &B) {
  return A.Coords[0] * B.Coords[0] + A.Coords[1] * B.Coords[1] +
         A.Coords[2] * B.Coords[2];
}

TVector operator*(const TVector &V, const TMatrix &M) {
  int I, J;
  TVector R;

  for (I = 0; I < 4; I++) {
    double Value = 0;
    for (J = 0; J < 4; J++)
      Value += V.Coords[J] * M.Elements[I][J];
    R.Coords[I] = Value;
  }
  return R;
}

TVector operator/(const TVector &V, double Value) {
  TVector R(V);
  double InvValue;
  if (Value) {
    InvValue = 1 / Value;
    R *= InvValue;
  }
  return R;
}

TVector &TVector::Normalize(void) {
  double L, InvL;

  L = Length();
  if (L) {
    InvL = 1 / L;
    Coords[0] *= InvL;
    Coords[1] *= InvL;
    Coords[2] *= InvL;
  }
  return *this;
}

void TVector::RotateX(double Angle) {
  double C = (double)cos(Angle);
  double S = (double)sin(Angle);
  TMatrix X(1, 0, 0, 0, 0, C, S, 0, 0, -S, C, 0, 0, 0, 0, 1);
  *this *= X;
}

void TVector::RotateY(double Angle) {
  double C = (double)cos(Angle);
  double S = (double)sin(Angle);
  TMatrix Y(C, 0, -S, 0, 0, 1, 0, 0, S, 0, C, 0, 0, 0, 0, 1);
  *this *= Y;
}

void TVector::RotateZ(double Angle) {
  double C = (double)cos(Angle);
  double S = (double)sin(Angle);
  TMatrix Z(C, S, 0, 0, -S, C, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
  *this *= Z;
}

void TVector::Rotate(TVector *Dir, double Angle) {
  double C = (double)cos(Angle);
  double S = (double)sin(Angle);

  Dir->Normalize();
  double X = Dir->Coords[0];
  double Y = Dir->Coords[1];
  double Z = Dir->Coords[2];

  TMatrix R(X * X + C * (1 - X * X), X * Y * (1 - C) + Z * S,
            X * Z * (1 - C) - Y * S, 0, X * Y * (1 - C) - Z * S,
            Y * Y + C * (1 - Y * Y), Y * Z * (1 - C) + X * S, 0,
            X * Z * (1 - C) + Y * S, Y * Z * (1 - C) - X * S,
            Z * Z + C * (1 - Z * Z), 0, 0, 0, 0, 1);
  *this *= R;
}

// ===========================
// Matrix class implementation
// ===========================

TMatrix::TMatrix(void) { Clear(); }

TMatrix::TMatrix(double Value) { Clear(Value); }

TMatrix::TMatrix(const TMatrix &M) {
  memmove(&Elements, &M.Elements, sizeof(double) * 16);
}

TMatrix::TMatrix(double E00, double E01, double E02, double E03, double E10,
                 double E11, double E12, double E13, double E20, double E21,
                 double E22, double E23, double E30, double E31, double E32,
                 double E33) {
  Elements[0][0] = E00;
  Elements[0][1] = E01;
  Elements[0][2] = E02;
  Elements[0][3] = E03;
  Elements[1][0] = E10;
  Elements[1][1] = E11;
  Elements[1][2] = E12;
  Elements[1][3] = E13;
  Elements[2][0] = E20;
  Elements[2][1] = E21;
  Elements[2][2] = E22;
  Elements[2][3] = E23;
  Elements[3][0] = E30;
  Elements[3][1] = E31;
  Elements[3][2] = E32;
  Elements[3][3] = E33;
}

void TMatrix::Clear(void) {
  memset(&Elements, 0, sizeof(double) * 16);
  Elements[3][3] = 1;
}

void TMatrix::Clear(double Value) {
  Clear();
  Elements[0][0] = Elements[1][1] = Elements[2][2] = Value;
}

TMatrix &TMatrix::operator=(const TMatrix &M) {
  memmove(&Elements, &M.Elements, sizeof(double) * 16);
  return *this;
}

TMatrix &TMatrix::operator*=(const TMatrix &M) {
  int I, J, K;
  TMatrix R;

  for (I = 0; I < 4; I++)
    for (J = 0; J < 4; J++) {
      double Value = 0;
      for (K = 0; K < 4; K++)
        Value += Elements[I][K] * M.Elements[K][J];
      R.Elements[I][J] = Value;
    }
  *this = R;
  return *this;
}

TMatrix operator*(const TMatrix &A, const TMatrix &B) {
  int I, J, K;
  TMatrix R;

  for (I = 0; I < 4; I++)
    for (J = 0; J < 4; J++) {
      double Value = 0;
      for (K = 0; K < 4; K++)
        Value += A.Elements[I][K] * B.Elements[K][J];
      R.Elements[I][J] = Value;
    }
  return R;
}

void TMatrix::RotateX(double Angle) {
  double C = (double)cos(Angle);
  double S = (double)sin(Angle);
  TMatrix X(1, 0, 0, 0, 0, C, S, 0, 0, -S, C, 0, 0, 0, 0, 1);
  *this *= X;
}

void TMatrix::RotateY(double Angle) {
  double C = (double)cos(Angle);
  double S = (double)sin(Angle);
  TMatrix Y(C, 0, -S, 0, 0, 1, 0, 0, S, 0, C, 0, 0, 0, 0, 1);
  *this *= Y;
}

void TMatrix::RotateZ(double Angle) {
  double C = (double)cos(Angle);
  double S = (double)sin(Angle);
  TMatrix Z(C, S, 0, 0, -S, C, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
  *this *= Z;
}

void TMatrix::MoveTo(double X, double Y, double Z) {
  Elements[3][0] = X;
  Elements[3][1] = Y;
  Elements[3][2] = Z;
}

void TMatrix::MoveTo(TVector *V) {
  Elements[3][0] = V->Coords[0];
  Elements[3][1] = V->Coords[1];
  Elements[3][2] = V->Coords[2];
}

void TMatrix::Move(double X, double Y, double Z) {
  Elements[3][0] += X;
  Elements[3][1] += Y;
  Elements[3][2] += Z;
}

void TMatrix::Move(TVector *V) {
  Elements[3][0] += V->Coords[0];
  Elements[3][1] += V->Coords[1];
  Elements[3][2] += V->Coords[2];
}

void TMatrix::Rotate(TVector *Dir, double Angle) {
  double C = (double)cos(Angle);
  double S = (double)sin(Angle);

  Dir->Normalize();
  double X = Dir->Coords[0];
  double Y = Dir->Coords[1];
  double Z = Dir->Coords[2];

  TMatrix R(X * X + C * (1 - X * X), X * Y * (1 - C) + Z * S,
            X * Z * (1 - C) - Y * S, 0, X * Y * (1 - C) - Z * S,
            Y * Y + C * (1 - Y * Y), Y * Z * (1 - C) + X * S, 0,
            X * Z * (1 - C) + Y * S, Y * Z * (1 - C) - X * S,
            Z * Z + C * (1 - Z * Z), 0, 0, 0, 0, 1);

  *this *= R;
}
#endif
