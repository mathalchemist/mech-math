// word count
// (C) SEGMENTATION FAULT Software Inc, 2003

#include <stdio.h>

// MAIN FUNCTION
int main()
{
    FILE * F = fopen("text.dat", "r");
    if (!F) return 0;
    int WordCount = 0;
    bool WordStarted = false;
    while (true)
    {
	int C = fgetc(F);
	if (C >= 'A' && C <= 'z') WordStarted = true;
	else
	    if (WordStarted) { WordCount++; WordStarted = false; }
	if (C == EOF || feof(F))
	{
	    if (WordStarted) WordCount++;
	    break;
	}
    }
    fclose(F);
    printf("\nWord count: %d.\n", WordCount);    
    return 0;	
}
