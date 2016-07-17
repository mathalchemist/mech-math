// =====================================
// Matrix Class test
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

// ===========================
// Matrix class implementation
// ===========================

#include "matrix.h"

double TMatrix::Det()
{
  if (W != H) throw Exception("Matrix in not square");
  TMatrix D(*this);
  double det = 1.0;
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
    if (MaxIndex == -1)
    {
//      D.Print();
      return 0;
    }
    // Swapping line [C] and [MaxIndex]
    if (C != MaxIndex)
    {
      D.SwapRows(C, MaxIndex);
      det = -det;
    }
    // Now [C][C] is nonzero.
    double Norm = D.MATR(C, C);
    for (int j = C + 1; j < D.H; j++) // j-th column
    {
      double Lambda = D.MATR(C, j) / Norm;
      D.SubRows(j, C, Lambda); 
    }
  }
//  D.Print();
  for (int i = 0; i < D.W; i++) det *= D.MATR(i,i);
  return det;
}

