#include <math.h>
#include <stdio.h>

#define U(x,t) u[(t-1)*M+x-1]

const double Pi = 4.0*atan(1);

const double a = 1;

const int N = 10000;
const int M = 2000; // x 

// u(t,x) = u(0,x-at)

const double T = 5.0;
const double A = 4.0;
double tau = T/double(N);
double h = A/double(M);
 

double X(int m) { return A*double(m)/double(M); }
//double T(int n) { return T*double(n)/double(N); }

double u_0(double x) { return cos(2.0*Pi*x); }

int main()
{
    FILE*F= fopen("data.mp", "w+");
    fprintf(F, "numeric u; u:=2cm;\n");
    double* u = new double[M*N];
    for (int m = 1; m <= M; m++) U(m,1) = u_0(X(m-1));
    
    for (int n = 1; n < N; n++)
    for (int m = 1; m <= M; m++)
    {
        int mp1; if (m == M) mp1 = 1; else mp1 = m+1;
        int mm1; if (m == 1) mm1 = M; else mm1 = m-1;
        U(m,n+1) = a*tau/(2.0*h)*(U(mp1,n)-U(mm1,n)) + U(m,n);
    }
    printf("done\n");

    double c = 2.0;
    for (int n = 1; n < N/4; n+=(N/10))
    {
    double uu0 = U(1,n);
    double av0 = fabs(uu0);
    if (av0 > c) uu0 = c*uu0/av0;
    fprintf(F, "\ndraw (%1.3lf*u, %1.3lf*u)\n ", X(0), uu0);
    
    for (int m = 1; m < M-1; m++)
    {
        double uu = U(m,n);
        double av = fabs(uu);
        if (av > c) uu = c*uu/av;
        fprintf(F, "--(%1.3lf*u, %1.3lf*u)\n ", X(m), uu);
    }
    fprintf(F," withpen pencircle scaled .1pt;\n");
    }
    delete u;
    
    return 0;
}
