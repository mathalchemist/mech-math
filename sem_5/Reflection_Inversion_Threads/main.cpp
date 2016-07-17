#include <stdio.h>
#include <string.h>
#include <time.h>

#include "func.h"

double f(int i, int j)
{
    if (i==j) return 1.0+1.0 / double(i+j+1); else
    return 1.0/double(i+j+1.0);
}

void FillMatrix(double * a, int n)
{
    for (int i = 0; i < n; i++)
	for (int j = 0; j < n; j++) a[i*n+j] = f(i,j);
}

int main()
{
    int n, TC;
    //char fn[256];
    printf("Input dimension (n): ");
    scanf("%d", &n);
    if (n <= 0) return -1;
    printf("Input number of the threads: ");
    scanf("%d", &TC);
    if (n <= 0) return -2;
    //printf("Input source file name (or 'func' to use function): ");
    //scanf("%255s", fn);
    double * a = new double[n*n];
    double * ac = new double[n*n];
    double * ai = new double[n*n];
    printf("Filling...\n");
    /* if (fn[0] == 'f') */ FillMatrix(a, n); /* else ReadMatrix(fn, a, n, n); */
    printf("Saving...\n");
    for (int i = 0; i < n*n; i++) ac[i] = a[i];
    printf("Inverting...\n");
    clock_t ts = clock();
    double ThreadsTime = 0;
    if (!S_Reflect(TC, a, n, ai, &ThreadsTime)) { printf("Bad matrix!\n"); return -3; }
    clock_t te = clock();
	
//    printf("Result:\n"); PrintMatrix(ai, 0, n);
    printf("Calculating error...\n");
    printf("%d-THREADED: E=%1.20lf, T=%1.4lf s.\n", TC, CalcError(ac, ai, n), Time(ts, te) + ThreadsTime);
    delete a;
    delete ac;
    delete ai;
    return 0;
}
