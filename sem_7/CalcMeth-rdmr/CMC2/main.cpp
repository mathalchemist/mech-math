#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <windows.h>

double alpha;

double gi(double x) { return sqrt(x*x*x+1.0/x); }

int N = 10000;

double Int_0(double x)
{
  double n = double(N);
  double r = 0;
    for (int i = 1; i <= N; i++)
  {
    double a = double(i)*x/n;
    double b = double(i+1)*x/n;
    r += gi(.5*a+.5*b)*(b-a);
  }
  return r;
}

double Int_1(double x)
{
  double n = double(N);
  double r = 0;
    for (int i = 1; i <= N; i++)
  {
    double a = double(i)*x/n;
    double b = double(i+1)*x/n;
    r += 0.5*(gi(a)+gi(b))*(b-a);
  }
  return r;
}

double Int_2(double x)
{
  double n = double(N);
  double r = 0;
    for (int i = 1; i <= N; i++)
  {
    double a = double(i)*x/n;
    double b = double(i+1)*x/n;
    r += (b-a)/6.0*(gi(a)+4*gi(0.5*a+0.5*b)+gi(b));
  }
  return r;
}

double Int(double x) { return Int_2(x); }
double f(double x) { return Int(x)-alpha*alpha -5*sqrt(x); }
/*double df(double x) { double h = 1e-5; return (f(x+h)-f(x))/h; } */
double g(double x) { return Int(x)-alpha*alpha-5*sqrt(x); }



void graph()
{
  double prec = 1e-10;
  double x_0 = 0; 
  FILE * gf = fopen("t.mp", "w+"); 
  for (alpha = 0; alpha < 100; alpha+=0.3)
  {
  double i = 0;

    double x1 = 0.1;
    double x2 = 100;
    if (g(x1)*g(x2) > 0) { printf("wrong starting conditions\n"); continue; }
    double m;
    while (true)
    {
    m = (x1+x2)/2.0;
    if (g(m)*g(x1) < 0) x2 = m; else x1 = m;
    if (fabs(g(m)) < 1e-5) break;
    i++;
    if (i > 100) { printf("cycled!\n"); break; }
  }
    fprintf(gf, "--(%1.4lf*u, %1.4lf*u)\n", alpha,m);
  }
  fclose(gf);
}


int main()
{
  int i = 0;
  double x = 0;
  double prec = 1e-8;

  /*printf("Quadrature Runge test...\n");
  double XX = 100;
  N = int(100000.0*XX);
  double I1 = Int(XX);
  N *= 2;
  double I2 = Int(XX);
  printf("Abs. error: %lf. Rel. error: %g. Value: %1.5lf, ", fabs(I1-I2), fabs(I1-I2)/I2, I1);
  getch();
  return 0;*/

  printf("enter x_0: ");
  double x_0;
  scanf("%lf", &x_0);

  printf("enter parameter Alpha: ");
  scanf("%lf", &alpha);
  N = int(10000.0*pow(alpha,4/5));

  x = x_0;
//  if (fabs(df(x))>1) { printf("unstable solution, try again\n"); return 0; }
  double mu = 1.0/(x_0*x_0);
  while (true)
  {
    double new_x = x - mu*f(x);
    if (fabs(new_x-x) < prec) break;
    x = new_x;
    i++;
  //  printf("%1.15lf\n", x);
  //if (getch()==27) break;
	if(i >10000) break;
  }
  printf("Parameter: %1.20lf\n", alpha);
  printf("Precision: %g\n", prec);
  printf("Error: %g\n", g(x));

  printf("Simple iteration: x = %1.20lf, \nIterations: %d\n", x, i);
  
  i  = 0;
  double x1 = 0.1;
  double x2 = 100;
  if (g(x1)*g(x2) > 0) { printf("wrong starting conditions\n"); return -1; }
  double m;
  while (true)
  {
    m = (x1+x2)/2.0;
    if (g(m)*g(x1) < 0) x2 = m; else x1 = m;
    if (fabs(g(m)) < prec) break;
    i++;
    //printf("%d\n",i);
  if (i > 100) { printf("cycled!\n"); break; }
  }
  printf("Bisection method: x = %1.20lf, \nIterations: %d\n", m, i);
  printf("Error: %g\n", g(m));

  i = 0;
  x = x_0;
  double xp = x_0+0.1;
  while (true)
  {
    double xn = x - g(x)*(x-xp)/(g(x)-g(xp));
    xp = x;
    x = xn;
    if (fabs(g(x)) < prec) break;
    i++;
  //printf("%d\n", i);
  if (kbhit()) break;
  if (i > 100) { printf("cycled!\n"); break; }
  }
  printf("Newton's method: x = %1.20lf, \nIterations: %d\n", x,i);
  printf("Error: %g\n", g(x));
  getch();
//  graph();
//  getch();
  return 0;


}