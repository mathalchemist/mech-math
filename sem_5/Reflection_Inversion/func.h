#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// read matrix from file FileName
bool ReadMatrix(char *FileName, double *a, int m, int n);
// debug method: display matrix
void PrintMatrix(double *arr, int k, int n);
void PrintVector(double *arr, int n);

// Make Gauss reverse solution ('a' is Upper-Triangular!!!)
void GaussReverse(double *a, double *ai, int n, int col);
// Solve system using Reflection method
bool S_Reflect(double *arr, int n, double *ai);

double CalcError(double *a, double *ai, int n);

