#ifndef __SS_H_INCLUDED__
#define __SS_H_INCLUDED__

#include <stdio.h>
#include <math.h>

#define Ass(i,j) a[(i)*nn+(j)]

void PrintMatrix(double *a, double *b, int nn);
void PrintX(double*x,int nn);
bool SolveSystem(int nn, double *a, double *b, double *x);

#endif
