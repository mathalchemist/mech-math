#ifndef ___i___
#define ___i___

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef long double real;
// read matrix from file FileName
bool ReadMatrix(char *FileName, real *a, int m, int n);
// debug method: display matrix
void PrintMatrix(real *arr, int k, int n);

// Solve system using Reflection method
void S_Reflect(real acc, real *arr, int n, real *q, real *ev);

real CalcError(real *a, real *ev, int n);

#endif

