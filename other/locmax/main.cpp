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
    int r =locmax(fn);
    if (r >= 0)
    {
	printf("File '%s' contains %d local maximum(s).\n", fn, r);
	return 0;
    }
    else { printf("Error code: [%d].\n", r); return r; }
    return 0;
}
