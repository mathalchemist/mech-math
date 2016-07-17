#include "function.h"

int symarr(char * filename, int n)
{
    FILE * F = fopen(filename, "r");
    
    if (!F) return -1;                  // error

    double* a = new double [n*n];
    for (int i = 0; i < n*n; i++)
    {
	if (fscanf(F, "%lf", &a[i]) < 1) { delete a; return -2; }
    }
    for (int i = 0; i < n; i++)
	for (int j = 0; j < n; j++)
	    if (a[i*n+j] != a[j*n+i]) { delete a; return 0; }
    delete a; return 1;
}
