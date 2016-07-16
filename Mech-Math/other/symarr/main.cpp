#include <stdio.h>

#include "function.h"

int main()
{
    char fn[256];
    printf("Enter source filename:\n");
    scanf("%255s", fn);
    
    if (!fn[0]) // empty filename string
    {
	printf("wrong filename.\n");
	return -3;
    }
    int n;
    printf("Input array size: \n"); scanf("%d", &n);
    if (n < 1) { printf("Wrong size! \n"); return -4; }
    int r = symarr(fn, n);
    if (r == 1) printf("Array in file '%s' is symmetrical.\n", fn, r);
    else if (r == 0) printf("Array in file '%s' is NOT symmetrical.\n", fn, r);
         else { printf("Error code: [%d].\n", r); return r; }
    return 0;
}
