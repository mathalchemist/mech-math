#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <pthread.h> 

// read matrix from file FileName
bool ReadMatrix(char * FileName, double * a, int m, int n);
// debug method: display matrix
void PrintMatrix(double * arr, int k, int n);
// Make Gauss reverse solution ('a' is Upper-Triangular!!!)
void GaussReverse(double * a, double * ai, int n, int col);
// Calculate inverse matrix using reflection
bool S_Reflect(int ThreadCount, double * arr, int n, double * ai, double * TimeUsed);
// Returns time in seconds (double precision)
double Time(clock_t start, clock_t end);
// Calculates difference btw A*A^{-1} and E
double CalcError(double * a, double * ai, int n);

// ================================================================
class CCommonData // class representing thread data
{
public:
    double Time;
    double * m; // matrix pointer
    double * x; // vector X pointer
    int n;	// total size of matrix
    int k;	// current step
};

class CThreadData // class representing thread data
{
public:
    pthread_t id;
    int cs; // start column
    int ce; // end column
    CCommonData * pCD; // ptr to Common Data
};
