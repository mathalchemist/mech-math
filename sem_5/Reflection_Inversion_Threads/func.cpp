#include "func.h"

#define A(i,j) a[(i)*n+(j)]
#define AI(i,j) ai[(i)*n+(j)]

//#define __printf_debug__

const double eps = 1e-30;

void * Thread(void * Ptr) // multiplies CPT cols of CD.m by U(x) 
{
    clock_t ts = clock();
    CThreadData * ThreadData = (CThreadData *) Ptr;
    int cs = ThreadData->cs;  // start
    int ce = ThreadData->ce;  // end
    double * m = ThreadData->pCD->m;
    double * x = ThreadData->pCD->x;
    int k = ThreadData->pCD->k;
    int n = ThreadData->pCD->n;
    
    for (int col = cs; col < ce; col++) // cycle through cols of A
    {
        double dp = 0.0;
        for (int i = k; i < n; i++) dp += x[i]*m[i*n+col];
	dp *= 2.0; for (int i = k; i < n; i++) m[i*n+col] -= x[i]*dp;
    }
#ifdef __printf_debug__
    printf("Cols [%d..%d] calculated\n", cs, ce-1);
#endif
    ThreadData->pCD->Time += Time(ts, clock());
    return 0;
}

bool S_Reflect(int ThreadCount, double * a, int n, double * ai, double * TimeUsed)
{
    double * x = new double[n];   // vector X
    
    CThreadData * T = new CThreadData[ThreadCount];
    CCommonData CD;  // common data
    CD.Time = 0.0;
    CD.x=x;
    CD.n=n;

    int FullThreads, iFullThreads;
    
    for (int i = 0; i < ThreadCount; i++) T[i].pCD = &CD;
    // thread params for inverted matrix
    int iRest = n % ThreadCount;
    if (!iRest) iFullThreads = ThreadCount; else iFullThreads = ThreadCount-1;
    int iColsPerThread = n/iFullThreads; // cols per thread
    
    double s, t, Nx, Na; // Norm of vector    
    
    for (int i=0;i<n;i++) for (int j=0;j<n;j++) ai[i*n+j]=(i==j);
	
    for (int k = 0; k < n-1; k++) // steps. let's begin...
    {
#ifdef __printf_debug__
	printf("Reflect-step: %d\n", k);    
#endif
	s = 0.0; 
	for (int i = k+1; i < n; i++) { x[i] = A(i,k); s += x[i]*x[i]; }
	t = A(k,k);
	Na = sqrt(t*t + s); // norm of vector a_1
	t -= Na;
	x[k] = t; // x[k] = a_1 - |a_1|*e_1
	
	Nx = sqrt(t*t+s);
	if (Nx < eps) return false;
	Nx = 1.0 / Nx; for (int i=k;i<n;i++) x[i] *= Nx;
	
	CD.k = k;
	
	// calculate matrix A
	CD.m = a;
	
	int TotalCols = n - (k+1); // total cols
	int Rest = TotalCols % ThreadCount;
	if (!Rest) FullThreads = ThreadCount; else FullThreads = ThreadCount-1;
	int ColsPerThread = TotalCols/FullThreads; // cols per thread
	
	for (int i = 0; i < FullThreads; i++) // run "full" threads
	{
	    T[i].cs = k+1 + i*ColsPerThread;
	    T[i].ce = k+1 + (i+1)*ColsPerThread;
	    pthread_create(&(T[i].id), 0, Thread, &T[i]); 
	}
	if (Rest) // the rest...
	{
	    T[FullThreads].cs = k+1 + FullThreads*ColsPerThread;
	    T[FullThreads].ce = n;
	    pthread_create(&(T[FullThreads].id), 0, Thread, &T[FullThreads]); 
	}
	for (int i = 0; i < ThreadCount; i++) pthread_join(T[i].id, 0);

	A(k,k) = Na;
	
	// Calculate matrix AI
	CD.m = ai;
	
	for (int i = 0; i < iFullThreads; i++) // run "full" threads
	{
 	    T[i].cs = i*iColsPerThread;
	    T[i].ce = (i+1)*iColsPerThread;
	    pthread_create(&(T[i].id), 0, Thread, &T[i]); 
	}
	if (iRest > 0) // the rest...
	{
	    T[iFullThreads].cs = iFullThreads*iColsPerThread;
	    T[iFullThreads].ce = n;
	    pthread_create(&(T[iFullThreads].id), 0, Thread, &T[iFullThreads]); 
	}
	for (int i = 0; i < ThreadCount; i++) pthread_join(T[i].id, 0); 
    }
    delete x;
    delete T;
    
    for (int k = 0; k < n; k++)
    { 
#ifdef __printf_debug__
	printf("Gauss reverse step: %d\n", k);    
#endif
	GaussReverse(a, ai, k, n);
    }
    * TimeUsed = CD.Time;
    
    return true;
}

void GaussReverse(double * a, double * ai, int col, int n)
{
    for (int k = 1; k <= n; k++)
    {
	double koeff = AI(n-k, col);
	for (int i = n-k+1; i < n; i++) koeff -= AI(i, col)*A(n-k,i);
	AI(n-k,col) = koeff / A(n-k,n-k);
    }
}

void PrintMatrix(double * arr, int k, int n)
{
    for (int i = k; i < n; i++)
    {
	for (int j = k; j < n; j++) printf("%1.5lf ", arr[i*n+j]);
	printf("\n");
    }
}

bool ReadMatrix(char * fn, double * array, int m, int n)
{    
    FILE * F = fopen(fn, "r");
    if (!F) return false;
    int c = 0;
    while (!feof(F))
    {
	double x;
	int r = fscanf(F, "%lf", &x);
	if (r != 1) { fclose(F); return false; }
	array[c] = x;
	c++;
    }
    if (c < n*m) return false;
    fclose(F);
    return true;
}

double Time(clock_t start, clock_t end)
{
    return double(end-start)/double(CLOCKS_PER_SEC);
}

double CalcError(double * a, double * ai, int n)
{
    double er = 0.0;
    for (int i = 0; i < n; i++)
	for (int j = 0; j < n; j++)
	{
	    double s = 0.0;
	    for (int k = 0; k < n; k++) s += a[i*n+k]*ai[k*n+j];
	    if (i == j) er += fabs(s-1.0); else er += fabs(s); 
	}
    return er;
}
