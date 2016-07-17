#include "function.h"

int locmax(char * filename)
{
    FILE * F = fopen(filename, "r");
    if (!F) return -1;                  // error
    double xl, xm, xr;  // left, middle, right
    int n = 0; // counter
    int r = 0; // result 
    while (!feof(F))
    {
	xl = xm;
	xm = xr;
	if (fscanf(F, "%lf", &xr) < 1) { fclose(F); return -2; }
	n++;
	if (n == 2) // special case: start of sequence
	{
	    if (xm > xr) r++;
	}
	if (n > 2) // generic case
	{
	    if (xl < xm && xm > xr) r++;
	} 
    }
    if (xr > xm) r++; // if last is greater than previous
    fclose(F);
    return r;    
}
