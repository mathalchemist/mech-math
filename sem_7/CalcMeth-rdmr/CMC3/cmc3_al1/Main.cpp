#include <stdio.h>
//#include <conio.h>
#include <math.h>

double alpha = -0.5;

double dx(double x,double y) { return y+x*x*x-alpha*x*x*x*x*x; }
double dy(double x, double y) { return -x+y*y*y-alpha*y*y*y*y*y; }
double l(double a, double b) { return sqrt(a*a+b*b); }
double l2(double a, double b) { return a*a+b*b; }
double d(double x1, double y1, double x2, double y2) { return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)); }

int main()
{

double C = 1.7;
for (double k = -C; k < C; k+= (0.1*C))
{
  double x_0 = C;
  double y_0 = k;
  double x = x_0;
  double y = y_0;
  double c = 0.003;
  const int NN = 5;
  const int NNN = 5000;

  c*= fabs(alpha)/alpha;

  printf("draw (%1.4lf*u, %1.4lf*u)\n", x,y);
  for (int i = 0; i < NNN; i++)
  {
    double ddx = dx(x,y);
    double ddy = dy(x,y);
    double len = l(ddx,ddy);
    x += ddx*c;
    y += ddy*c;
    if (!(i%NN)) printf("--(%1.4lf*u, %1.4lf*u)\n", x,y);
    if (l2(x,y)>300) { printf("unstable point! \n"); return -1; }
  }
  printf("withpen pencircle scaled 1pt;\n");
 }
for (double k = -C; k < C; k+= (0.1*C))
{
  double x_0 = k;
  double y_0 = -C;

  double x = x_0;
  double y = y_0;
  double c = 0.003;
  const int NN = 5;
  const int NNN = 5000;

  c*= fabs(alpha)/alpha;

  printf("draw (%1.4lf*u, %1.4lf*u)\n", x,y);
  for (int i = 0; i < NNN; i++)
  {
    double ddx = dx(x,y);
    double ddy = dy(x,y);
    double len = l(ddx,ddy);
    x += ddx*c;
    y += ddy*c;
    if (!(i%NN)) printf("--(%1.4lf*u, %1.4lf*u)\n", x,y);
    if (l2(x,y)>300) { printf("unstable point! \n"); return -1; }
  }
  printf("withpen pencircle scaled 1pt;\n");
 }
  return 0;
}
