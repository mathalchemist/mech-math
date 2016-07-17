#include <math.h>
#include <stdio.h>
#include <string.h>

#define U(x,t) u[(t-1)*M+x-1]
#define A(i,j) a[(i-1)*M+j-1]
#define B(i) b[i-1]
//#define AC(i,j) a[(i-1)*M+j-1]
//#define BC(i) b[i-1]
#define X(i) x[i-1]

const double Pi = 4.0*atan(1);

const double aa = 0.2;

const int N = 400;
const int M = 200; // x 

// u(t,x) = u(0,x-at)

const double T = 5.0;
const double x_max = 4.0;
double tau = T/double(N);
double h = x_max/double(M);

double xx(int m) { return x_max*double(m)/double(M); }
double xs(int m, int n) { return xx(m)+aa*T*double(n)/double(N); }
//double T(int n) { return T*double(n)/double(N); }

double u_0(double x) { return cos(2.0*Pi*x); }

void pm(double * a);
bool solve(double * a, double * b, double *x);

int main()
{
    FILE*F= fopen("data.mp", "w+");
    fprintf(F, "numeric u; u:=2cm;\n");
    double * u = new double[M*N];
    
// calculation
    double * a = new double[M*M];
    double * b = new double[M];
    double * x = new double[M];
  //  double * ac = new double[M*M];
  //  double * bc = new double[M];
    
    for (int m = 1; m <= M; m++) U(m,1) = u_0(xx(m-1));
      
    for (int n = 1; n < N; n++)
    {
        memset(a, 0, sizeof(double)*M*M);
        for (int m = 1; m <= M; m++)
        {
            int mm1 = m-1; if (m == 1) mm1 = M;
            int mp1 = m+1; if (m == M) mp1 = 1;

            A(m,mm1) = aa*tau;
            A(m,m) = 4*h;
            A(m,mp1) = -aa*tau;
        
            B(m) = 4*h*U(m,n) + aa*tau*U(mp1,n)-aa*tau*U(mm1,n);
        }

        
        if (!solve(a, b, x)) printf("bad: n=%d\n", n);
    
        for (int m = 1; m <= M; m++) U(m,n+1) = X(m);
    }

    //delete ac;
    //delete bc;
    delete a;
    delete b;
    delete x;

    printf("done\n");

    double em = 0;
    for (int m = 1; m <= M; m++)
{
	if (em < fabs(u_0(xs(m,N)) - U(m,N)) ) em = fabs(u_0(xs(m,N)) - U(m,N))/double(N);
}
	printf("\ne = %1.10lf\n", em);


// drawing

    double e = 0;
    for (int m = 1; m <= M; m++)  e+= (fabs(u_0(xs(m,N)) - U(m,N)))/double(M);
    printf("Sum error: %1.10lf\n", e);


    double c = 1.7;
    for (int n = 1; n < N; n+= N/20)
//  int n=3;
    {
        bool suxx = false;
        double uu0 = U(1,n);
        double av0 = fabs(uu0);
        if (av0 > c) { uu0 = c*uu0/av0; suxx = true; }
        fprintf(F, "\ndraw (%1.3lf*u, %1.3lf*u)\n ", xx(0), uu0);
    
        for (int m = 1; m < M; m++)
        {
            double uu = U(m,n);
            double av = fabs(uu);
            if (av > c) { uu = c*uu/av; suxx  = true; }
            fprintf(F, "--(%1.3lf*u, %1.3lf*u)\n ", xx(m-1), uu);
        }
        fprintf(F," withpen pencircle scaled .3pt;\n");
//        if (suxx) break; 
    }
    delete u;
    
    return 0;
}

void pm(double * a)
{
    printf("\n=====\n");
    for (int i = 1; i <= M; i++)
    {
        for (int j = 1; j <= M; j++)
            printf("%4.2lf ", A(i,j));
        printf("\n");
    }
}

bool solve(double * a, double * b, double *x)
{
//    pm(a);

    for (int k = 1; k <= M; k++)
    {

        double max = A(k,k);
        int mi = k;
        for (int i = k+1; i <= M; i++) if (fabs(A(i,k)) > max) { max = A(i,k); mi = i; }

        if (mi != k)
        {
            for (int j = k; j <= M; j++)
            {
                double t = A(k,j);
                A(k,j) = A(mi,j);
                A(mi,j) = t;
            }
            double t = B(k); B(k) = B(mi); B(mi) = t;
        }
        double p = A(k,k);
        if (fabs(p) < 1e-300) { return false; }

        for (int j = k; j <= M; j++) A(k,j) /= p;
        
        B(k) /= p;

        for (int i = k+1; i <= M; i++)
        {
            p = A(i,k);
            for (int j = k+1; j <= M; j++) A(i, j) -= A(k,j) *p;
            A(i,k) = 0;
            B(i) -= B(k)*p;
        }
    }
#ifdef ____debug___
    pm(a);
#endif
    // Gauss's "reverse step"
    for (int i = M; i > 0; i--)
    {
        double p = B(i);
        for (int j = i + 1; j <= M; j++) p -= X(j) * A(i, j);
        X(i) = p;
    }
    return true;
}



