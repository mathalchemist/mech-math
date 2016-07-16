#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

#define A(i,j) a[(i)*n+(j)]

// matrires & vectors with swapped coords
#define AS(i, j) a[(i) * n + index[j]] 
#define XS(i) x[index[i]]
#define BS(i) b[index[i]]

// MATRIX FUNCTION: A=(Hilbert(n)+E)
double f(int i, int j)
{
    if (i==j) return 1.0+1.0 / double(i+j+1); else
    return 1.0/double(i+j+1.0);
}

void PrintMatrix(double *a, double *b, int n)
{
    for (int i = 0; i < n; i++)
    {
	for (int j = 0; j < n; j++) printf("%1.5lf ", A(i, j));
	printf(" | %1.5lf\n", b[i]);
    }
} 
// print solution X
void PrintX(double*x,int n) { for (int i=0;i<n;i++) printf("%1.4lf ",x[i]); }

class CCommonData
{
public:
    double * a; // matrix pointer
    double * b; // 
    int * index;
    int n;	// total size of matrix
    int i;	// current step
};

class CThreadData // Data for each Thread
{
public:
    CCommonData * CD; 
    pthread_t id; // Thread identifier
    int rs; // start row
    int re; // end row
};

// thread function
void * Thread(void * Ptr) // subtracts one row from rows [rs..re) of matrix A 
{
    CThreadData * TD = (CThreadData *) Ptr;
    int rs = TD->rs;  // start
    int re = TD->re;  // end
    double * a = TD->CD->a; // matrix
    double * b = TD->CD->b; // vector (right part)
    int * index = TD->CD->index; // variable swap
    int i = TD->CD->i; // current step
    int n = TD->CD->n; // size

    for (int j = rs; j < re; j++)
    {
        double p = AS(j, i);
        for (int k = i; k < n; k++) AS(j, k) -= p * AS(i, k);
        b[j] -= p * b[i];
    }
    return 0;
}

// main solving function
bool SolveSystem(int n, double *a, double *b, double *x, int *index, int ThreadCount)
{
    CCommonData CD; // common thread data: pointers to matrix A and vector B,
                    // matrix size and current step 
    CD.a=a;
    CD.b=b;
    CD.index=index; // rows substtution array
    CD.n=n;
    CThreadData * T = new CThreadData[ThreadCount]; // array for thread data
    
    int i;
    for (int t = 0; t < ThreadCount; t++) T[t].CD = &CD; // fill pointers
                                                         // to common data
    
    for (i = 0; i < n; i++) index[i] = i; // init substitution: s=id

    for (i = 0; i < n; i++) // steps ('i' is inmber of step)
    {
	int k, j; // min index
	for (k = i, j = i; j < n; j++) // search for MAX element
	    if (fabs(AS(i, k)) < fabs(AS(i, j))) k = j;
	
	j = index[k]; // exchange lines
	index[k] = index[i];
	index[i] = j;
		
	double p = AS(i, i);
	if (fabs(p) < 1e-300) return false; // det A ~= 0 => very bad matrix :(	
	p = 1.0/p;
	for (k = i; k < n; k++) AS(i, k) *= p;
	b[i] *= p;
	
	CD.i = i; // set current step in common structure
	
	int Rows = n - (i+1); // total rows
	int Rest = Rows % ThreadCount; // rest number of rows

	int FullThreads = ThreadCount; 
	if (Rest) FullThreads = ThreadCount-1;
	int RowsPerThread = 0;
	if (FullThreads > 0) RowsPerThread = Rows/FullThreads;

	// run threads		
	for (int t = 0; t < FullThreads; t++) // run "full" threads
	{
	    T[t].rs = i+1 + t*RowsPerThread;
	    T[t].re = i+1 + (t+1)*RowsPerThread;
	    pthread_create(&(T[t].id), 0, Thread, &T[t]); 
	}
	
	if (Rest) // the rest...
	{
	    T[FullThreads].rs = i+1 + FullThreads*RowsPerThread;
	    T[FullThreads].re = n;
	    pthread_create(&(T[FullThreads].id), 0, Thread, &T[FullThreads]); 
	}
	// wait while threads finish their work
	for (int t = 0; t < ThreadCount; t++) pthread_join(T[t].id, 0);	
    }

    // Gauss's "reverse step"
    for (i = n - 1; i >= 0; i--)
    {
	double p = BS(i);
	for (int j = i + 1; j < n; j++) p -= x[j] * AS(i, j);
	x[i] = p/AS(i, i);
    }

    delete T; // we are accurate programmers: do not leave allocated memory :)
    return true;
}

// fill matrix and vector b using f().
// here we make solution x=(1,1...1)
void FillMatrix(double * a, double * b, int n)
{
    for (int i = 0; i < n; i++)
    {
	double s = 0;
	for (int j = 0; j < n; j++) { a[i*n+j] = f(i,j); s += f(i,j); }
	b[i] = s;
    }
}

/* return |A*x - b| */
double GetError(double *a, double *b, double *x, int n)
{
    double y = 0;
    for (int i = 0; i < n; i++)
    {
	double z = -b[i];
	for (int j = 0; j < n; j++) z += A(i, j) * x[j];
	y += z * z;
    }
    if (y > 0) y = sqrt(y);
    return y;
}

////////////////////////////////////////////////////////////////
int main()
{
    int n, TC;
    printf("Input dimension (n): ");
    scanf("%d", &n);
    if (n <= 0) return -1;
    printf("Input number of the threads: ");
    scanf("%d", &TC);
    if (n <= 0) return -2;
    double * a = new double[n*n];
    double * b = new double[n];
    double * x = new double[n];
    int * index = new int[n];
    double * ac = new double[n*n];
    double * bc = new double[n*n];
    FillMatrix(a, b, n);
    for (int i = 0; i < n*n; i++) ac[i] = a[i]; // copy of A
    for (int i = 0; i < n; i++) bc[i] = b[i]; // copy of B

//    PrintMatrix(a, b, n);

    timeval tv1;
    gettimeofday(&tv1, 0); // get current time
    if (!SolveSystem(n, a, b, x, index, TC))
    {
	printf("Bad matrix!\n");
	return -3;
    }
    timeval tv2;
    gettimeofday(&tv2,0); // again get current time

    // Get time in microseconds, then divide by 1.000.000
    double Time = (double(tv2.tv_sec)*1000000.0+double(tv2.tv_usec)-double(tv1.tv_sec)*1000000.0+double(tv1.tv_usec))/1000000.0;
	
    printf("Result:\n"); PrintX(x, n);
    printf("\n\nThreads: %d,\nError: %1.17lf,\nTime=%1.4lf sec.\n", TC, GetError(ac, bc, x, n), Time);
    delete a; delete b;
    delete x; delete index;    
    delete ac;
    return 0;
}
