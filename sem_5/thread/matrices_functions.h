#include <stdio.h>
#include <math.h>
#include <string.h>

int Read_Matrix (double *a, double *b, FILE *fp, int n);

void Init_A (double *A, int n, int block_n);
void Init_b (double *A, double *b, int n, int block_n);
void Init_E (double *E, int n);

void Print_Matrix (double *a, const char *s, int n);
void Print_Vector (double *b, const char *s, int n);

double Norm_Vector (double *b, int n);
double	Norm_Mtr(double *a, int m, int n);

void Mul_Mtr_Vector(double *a, double *b, double *c, int n);
void Mul_Mtr_Mtr(double *A, double *B, double *C, int n);
void Mul_Mtr_Mtr_Diag(double *A, double *D, int n);

double Residual (double *A, double *x, double *b, double *tmp, int n);
