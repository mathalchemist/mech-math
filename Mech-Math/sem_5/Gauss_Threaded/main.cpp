#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

#define A(i,j) a[(i)*n+(j)]

double f(int i, int j)
{
    return fabs(double(i-j));
    //if (i==j) return 1.0+1.0 / double(i+j+1); else return 1.0/double(i+j+1.0);
}

void PrintMatrix(double *a, double *b, int n)
{
    for (int i = 0; i < n; i++)
    {
	for (int j = 0; j < n; j++) printf("%1.4lf ", A(i, j));
	printf(" | %1.4lf\n", b[i]);
    }
} 
// print solution X
void PrintX(double*x,int n) { for (int i=0;i<n;i++) printf("%1.7lf ",x[i]); }

class CCommonData
{
public:
    double * a; // matrix pointer
    double * b; // 
    int n;	// total size of matrix
    int k;	// current step
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
    int k = TD->CD->k; // current step
    int n = TD->CD->n; // size

    for (int i = rs; i < re; i++)
    {
        double p = A(i, k);
        for (int j = k; j < n; j++) A(i, j) -= p * A(k, j);
        b[i] -= p * b[k];
    }
    return 0;
}

// main solving function
bool SolveSystem(int n, double *a, double *b, double *x, int ThreadCount)
{
    CCommonData CD; 
    CD.a=a;
    CD.b=b;
    CD.n=n;
    CThreadData * T = new CThreadData[ThreadCount]; // array for thread data
    
    for (int t = 0; t < ThreadCount; t++) T[t].CD = &CD; // fill pointers
    
    for (int k = 0; k < n; k++) // steps
    {
	double max = 0;
	int MR = k;
	for (int i = k+1; i < n; i++)
	    if (fabs(A(i,k)) > max) { MR = i; max = fabs(A(i,k)); }
	
	if (MR != k)
	{
	    for (int j = k; j < n; j++)
	    {
		double tmp = A(k,j);
		A(k,j) = A(MR,j);
		A(MR,j) = tmp;
	    }
	    double tmpb = b[k];
	    b[k] = b[MR];
	    b[MR] = tmpb;
	}
	double p = A(k, k);
	if (fabs(p) < 1e-100) return false;
	
	p = 1.0/p;
	for (int j = k; j < n; j++) A(k, j) *= p;
	b[k] *= p;
	
	CD.k = k; // set current step in common structure
	
	int Rows = n - (k+1); // total cols
	int RowsPerThread = Rows / ThreadCount;
	int Rest = Rows % ThreadCount;
	int FullThreads = ThreadCount;
	if (Rest)
	{
	    FullThreads--;
	    if (FullThreads* (RowsPerThread+1) <= Rows) RowsPerThread++;
	}
	int TTC = ThreadCount;	
	if (Rows < ThreadCount) // bad case!!!
	{
	    RowsPerThread = 1;
	    FullThreads = Rows;
	    Rest = 0;
	    TTC = FullThreads;
	}

	// run threads		
	for (int t = 0; t < FullThreads; t++) // run "full" threads
	{
	    T[t].rs = k+1 + t*RowsPerThread;
	    T[t].re = k+1 + (t+1)*RowsPerThread;
	    pthread_create(&(T[t].id), 0, Thread, &T[t]); 
	}
	
	if (Rest) // the rest...
	{
	    T[FullThreads].rs = k+1 + FullThreads*RowsPerThread;
	    T[FullThreads].re = n;
	    pthread_create(&(T[FullThreads].id), 0, Thread, &T[FullThreads]); 
	}
	// wait while threads finish their work
	for (int t = 0; t < TTC; t++) pthread_join(T[t].id, 0);	
    }
    printf("done!\n");

    // Gauss's "reverse step"
    for (int i = n - 1; i >= 0; i--)
    {
	double p = b[i];
	for (int j = i + 1; j < n; j++) p -= x[j] * A(i, j);
	x[i] = p/A(i, i);
    }

    delete T; 
    return true;
}

// fill matrix and vector b using f().
// here we make solution x=(1,1...1)
void FillMatrix(double * a, double * b, int n)
{
    for (int i = 0; i < n; i++)
    {
	double s = 0;
	for (int j = 0; j < n; j++) { a[i*n+j] = f(i,j); if (j%2) s += f(i,j); }
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
    double * ac = new double[n*n];
    double * bc = new double[n*n];
    FillMatrix(a, b, n);
    for (int i = 0; i < n*n; i++) ac[i] = a[i]; // copy of A
    for (int i = 0; i < n; i++) bc[i] = b[i]; // copy of B

//    PrintMatrix(a, b, n);

    timeval tv1;
    gettimeofday(&tv1, 0); // get current time
    if (!SolveSystem(n, a, b, x, TC))
    {
	printf("Bad matrix!\n");
	return -3;
    }
    timeval tv2;
    gettimeofday(&tv2,0); // again get current time

    // Get time in microseconds, then divide by 1.000.000
    double  s2 = double(tv2.tv_sec);
    double  s1 = double(tv1.tv_sec);
    double  ms2 = double(tv2.tv_usec);
    double  ms1 = double(tv1.tv_usec);
    double m = 1000000.0;
    double Time = ((s2*m+ ms2) - (s1*m+ms1))/m;
	
    printf("\n\nThreads: %d,\nError: %1.17lf,\nTime=%1.4lf sec.\n", TC, GetError(ac, bc, x, n), Time);

    FILE * F = fopen("result.txt", "w");
//    printf("Result:\n");
//    PrintX(x, n);

    for (int i = 0; i < n; i++) fprintf(F, "x_%d = %1.20lf\n", i+1, x[i]); 
    fclose(F);
    delete a; delete b;
    delete x;
    delete ac;
    return 0;
}
