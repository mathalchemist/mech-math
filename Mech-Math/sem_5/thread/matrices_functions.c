#include "matrices_functions.h"
#include <time.h>

#define N_MAX 6

int Read_Matrix(double *a, double *b, FILE *fp, int n)
{
  int i, k, count = 0;
  
  for (i = 0; i < n ; i++, b++) {
    for (k = 0; k < n ; k++, a++) 
      if (fscanf(fp,"%lf", a) != 1) return 0;
    if (fscanf(fp,"%lf", b) != 1) return 0;
    count += n + 1;
  }
  
  return count;
}

double Func(int i, int j)
{
  return i > j ? i+1 : j+1;
}

void Init_A(double *A, int n, int block_n)
{
  int i, j;
  double tmp;

  block_n = 1;
  for (i = 0; i < n ; i++) 
    for (tmp = 0, j = 0; j < n; j++, A++) 
      *A = Func(i, j);
  
}

void Init_b(double *A, double *b, int n, int block_n)
{
  int i, j;
  double tmp;
  block_n = 1;

  for (i = 0; i < n; i++, b++, A += n) {
    for (tmp = 0, j = 0; j < n; j += 2)
      tmp += A[j];
    *b = tmp;
  }

}

void Init_E(double *E, int n)
{
  int i;

  memset (E, 0, sizeof(double) * n * n);	
  for (i = 0; i < n; i++, E += n + 1)
    *E = 1;

}


void Print_Matrix(double *a, const char *s, int n)
{
  int i, j, nm;
  nm = n > N_MAX ? N_MAX : n;
      
  printf("Matrice %s is:\n", s);
  for (i = 0; i < nm; i++, a += n) {
    for(j = 0; j < nm; j++)
      printf("%14g ", a[j]);
    printf("\n");
  }

}

void Print_Vector(double *b, const char *s, int n)
{
  int i, nm;
  nm = n > N_MAX ? N_MAX : n;
      
  printf("Vector %s is:\n", s);
  for(i = 0; i < nm; i++)
    printf("%14g ", b[i]);
  printf("\n");

}

double Norm_Vector(double *b, int n)
{
  int i;
  double res = fabs(b[0]);
  
  for (i = 1; i < n; i++)
    if (fabs(b[i]) > res)
      res = fabs(b[i]);
      
  return res;      
}


double	Norm_Mtr(double *a, int m, int n)
{
	double	res = 0, tmp;
	int	i, j;

	for(i = 0; i < m; i++)
	{
		tmp = 0;
		for(j = 0; j < n; j++, a++)
			tmp += fabs(a[0]);
		if (tmp > res)
			res = tmp;
	}
	return res;
}

void Mul_Mtr_Vector (double *a, double *b, double *c, int n)
{
  int i, j;
  double tmp;
  
  for (i = 0; i < n; i++, c++) {
    for (tmp = 0, j = 0; j < n; j++, a++) 
      tmp += (*a) * b[j];
    *c = tmp;
  }
}  

void Mul_Mtr_Mtr(double *A, double *B, double *C, int n)
{
  int i, j, k;
  double tmp, *b = B;

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++, C++) {
      B = b + j;
      for (tmp = 0, k = 0; k < n; k++, B += n)
        tmp += A[k] * (*B);
      *C = tmp;
    }
    A += n;
  }
}

void Mul_Mtr_Mtr_Diag(double *A, double *D, int n)
{
  int i, j;

  for (i = 0; i < n; i++) 
    for (j = 0; j < n; j++, A++)
      *A *= D[j]; 
}

double Residual (double *A, double *x, double *b, double *tmp, int n)
{
  int i;
  
  Mul_Mtr_Vector (A, x, tmp, n);
  for (i = 0; i < n; i++)
    tmp[i] -= b[i];
  return Norm_Vector (tmp, n);
}
