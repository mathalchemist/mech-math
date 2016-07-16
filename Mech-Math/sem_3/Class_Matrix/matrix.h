// =====================================
// Matrix Class test
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

#ifndef __MATRIX_H_INCLUDED__
#define __MATRIX_H_INCLUDED__

#include <stdio.h>
#include <math.h>

// MATRIX Class interface

#define MATR(i,j) Arr[(j)*W+i]

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


