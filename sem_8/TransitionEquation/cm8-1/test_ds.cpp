#include <math.h>
#include <stdio.h>

#define U(x,t) u[(t-1)*M+x-1]

const double Pi = 4.0*atan(1);

const double a = 0.2;

const int N = 20000;
const int M = 2000; // x 

// u(t,x) = u(0,x-at)

const double T = 5.0;
const double A = 3.0;
double tau = T/double(N);
double h = A/double(M);
 

double X(int m) { return A*double(m)/double(M); }
double Xs(int m, int n) { return X(m)+A*T*double(n)/double(N); }
//double T(int n) { return T*double(n)/double(N); }

double u_0(double x) { return cos(2.0*Pi*x); }

int main()
{
    FILE*F= fopen("data.mp", "w+");
    fprintf(F, "numeric u; u:=2cm;\n");
    double* u = new double[M*N];
    for (int m = 1; m <= M; m++) U(m,1) = u_0(X(m-1));
    
    for (int n = 1; n < N; n++)
    {
    for (int m = 1; m <= M; m++)
    {
        int m1; if (m == M) m1 = 1; else m1 = m+1;

        U(m,n+1) = a*tau/h*(U(m1,n)-U(m,n)) + U(m,n);


//      fprintf(F, "(%1.4lf, %1.4lf)-- ", X(m), U(m,n));
    }
//  printf("\n");
//  printf("%d\n", n);
    }
    printf("done\n");

    double e = 0;
    for (int m = 1; m <= M; m++)  e+= (fabs(u_0(Xs(m,N)) - U(m,N)))/double(M);
    printf("Sum error: %1.10lf\n", e);

    
    for (int n = 1; n < N; n+=N/20)
    {
        fprintf(F, "\ndraw (%1.3lf*u, %1.3lf*u)\n ", X(0), U(1,n));
    
        for (int m = 1; m < M-1; m++) fprintf(F, "--(%1.3lf*u, %1.3lf*u)\n ", X(m), U(m,n));
        fprintf(F,";\n");


//        fprintf(F, "\ndraw (%1.3lf*u, %1.3lf*u)\n ", X(0), u_0(Xs(1,n)));
    
//        for (int m = 1; m < M-1; m++) fprintf(F, "--(%1.3lf*u, %1.3lf*u)\n ", X(m), u_0(Xs(m,n)));
//        fprintf(F," withcolor red;\n");
    }
    delete u;
    
    return 0;
}
