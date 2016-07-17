#include <stdio.h>
#include "plot_x11.h"
#include "math.h"
#include "ss.h"

#define F(i) fv[i-1]
#define X(i) x[i-1]
#define D(i) d[i-1]
#define A(i,j) aa[((i-1-1)*(n-2))+(j-1-1)]
#define B(i) bb[(i-1-1)]

// interpolation
int N = 0;
double a;
double b;

#define EM(i,j) em[(i-1)*4+(j-1)]

double em[16];
double exs[4]; // start
double exe[4]; // end
double eb[4];

double coeff_y = 1;
double coeff_x = 1;

double max = 0;
double min = 0;

double * x=0;
double * fv = 0;
double * d = 0;
double * aa = 0;
double * bb = 0;

double f(double x)
{
    return 1.0/(1.0+x*x); //sin(4.0*x) + .25*delta(x,0.5);
}

double disp = 1.0;
double delta(double x, double a)
{
    double p = (x-a)*(x-a)/disp; if (p > 100) return 0; else return exp(-p);
}

void FreeMem()
{
    delete x;
    delete fv;
    delete d;
    delete aa;
    delete bb;
}

void GetMem(int n)
{
    x = new double[n];
    fv = new double[n];
    d = new double[n];
    aa = new double[(n-2)*(n-2)];
    bb = new double[n-2];
}

double FD(int i,int j) 
{
    return (F(j)-F(i))/(X(j)-X(i));
}

void GetMaxMin()
{
    max = 0.0;
    min = 0.0;
    
    for (double t = a; t <=b; t+= 0.001)
    {
	double y = f(t);
	if (max < y) max = y;
	if (min > y) min = y;
    }
    max += 0.1;
    min -= 0.1;
}

double Par(int i, double t)
{
    if (i<=3)
    {
	return exs[0]*t*t*t+exs[1]*t*t+exs[2]*t+exs[3];
    }
    if (i>=N-2)
    {
	return exe[0]*t*t*t+exe[1]*t*t+exe[2]*t+exe[3];
    }
    double c1 = F(i);
    double c2 = D(i);
    double dx = X(i+1)-X(i);
    double c3 = (3*FD(i,i+1) -2*D(i)-D(i+1))/dx;
    double c4 = (D(i) + D(i+1)-2*FD(i,i+1))/(dx*dx);
    
    double st = t - X(i);
    return c1 + c2 * st + c3 * st*st + c4 * st*st*st;
}

void GetNodesAndValues(int n) // from formula
{
    for (int i = 1; i <= n; i++)
    {
     	X(i) = a + (double(i-1)/double(n-1)) * (b-a);
	F(i) = f(X(i));
    }
}

void MakeMatrix(int n)
{
    for (int i = 0; i < (n-2)*(n-2); i++) aa[i] = 0.0;
    for (int i = 0; i < (n-2); i++) bb[i] = 0.0;
    
    for (int i = 2; i <= n-1; i++)
    {
	if (i > 2) A(i,i-1) = X(i+1)-X(i);
	A(i,i) = 2.0*(X(i+1)-X(i-1));
	if (i < n-1) A(i,i+1) = X(i)-X(i-1);
	B(i) = 3.0*FD(i-1,i)*(X(i+1)-X(i)) + 3.0*FD(i,i+1)*(X(i)-X(i-1));
    }
}

void Interpolate(int n)
{
    FreeMem();
    GetMem(n);

    GetMaxMin();
    GetNodesAndValues(n);
    MakeMatrix(n);
    
    double * ss = new double[n-2];
    SolveSystem(n-2, aa,bb, ss);
    for (int i = 2; i < n; i++) D(i) = ss[i-2-1];
    delete ss;
    
    EM(1,1) = X(1)*X(1)*X(1);
    EM(1,2) = X(1)*X(1);
    EM(1,3) = X(1);
    EM(1,4) = 1.0;

    EM(2,1) = X(2)*X(2)*X(2);
    EM(2,2) = X(2)*X(2);
    EM(2,3) = X(2);
    EM(2,4) = 1.0;
    
    EM(3,1) = X(3)*X(3)*X(3);
    EM(3,2) = X(3)*X(3);
    EM(3,3) = X(3);
    EM(3,4) = 1.0;
    
    EM(4,1) = 3.0*X(3)*X(3);
    EM(4,2) = 2.0*X(3);
    EM(4,3) = X(3);
    EM(4,4) = 0.0;
    
    eb[0] = F(1);
    eb[1] = F(2);
    eb[2] = F(3);
    eb[3] = D(3);
    
    SolveSystem(4,em,eb,exs);

    EM(1,1) = X(n-2)*X(n-2)*X(n-2);
    EM(1,2) = X(n-2)*X(n-2);
    EM(1,3) = X(n-2);
    EM(1,4) = 1.0;

    EM(2,1) = X(n-1)*X(n-1)*X(n-1);
    EM(2,2) = X(n-1)*X(n-1);
    EM(2,3) = X(n-1);
    EM(2,4) = 1.0;
    
    EM(3,1) = X(n)*X(n)*X(n);
    EM(3,2) = X(n)*X(n);
    EM(3,3) = X(n);
    EM(3,4) = 1.0;
    
    EM(4,1) = 3.0*X(n-2)*X(n-2);
    EM(4,2) = 2.0*X(n-2);
    EM(4,3) = X(n-2);
    EM(4,4) = 0.0;
    
    eb[0] = F(n-2);
    eb[1] = F(n-1);
    eb[2] = F(n);
    eb[3] = D(n-2);
    
    SolveSystem(4,em,eb,exe);
}

int MX(double x)
{
    return int( (x-a)/(b-a)*double(nWWidth)*coeff_x - nWWidth/2*(coeff_x-1) );
}

int MY(double y)
{
    double m = .5*(max+min);
    double r = coeff_y * fabs(max-min);
    return int((m-y)/r*double(nWHeight) + nWHeight/2);
}

void DrawWindowContent()
{
    int w = nWWidth; int h = nWHeight;
    
    WSetColor (DARKGRAY);
    WFillRectangle (0, 0, w, h);
    WSetColor (BLACK);
    //axis
    WDrawLine(0, h/2, w, h/2);
    WDrawLine(w/2, 0, w/2, h);
    //arrows
    WDrawLine(w/2, 0, w/2-5,5);
    WDrawLine(w/2, 0, w/2+5,5);
    
    WDrawLine(w, h/2, w-5,h/2-5);
    WDrawLine(w, h/2, w-5,h/2+5);
    WSetColor(RED);
    WDrawString ("Q=quit, F1..F4 -- change scale, F5/F6 -- change node count", 10, 20);

    double diff = 0;
    for (int i = 1; i <= N-1; i++)
    {
    
	int k = i;
	if (i==1) k = 2;
	if (i==N-1) k = N-2; // edge conditions
	
	int ss = int((X(i)-a)/(b-a)*w);
	int se = int((X(i+1)-a)/(b-a)*w);
	
	for (int j = ss; j <= se; j++)
	{
	    double t1 = (double(j)/double(w))*(b-a)+a;
	    double t2 = (double(j+1)/double(w))*(b-a)+a;
	    WSetColor (GREEN);
	    
	    WDrawLine(MX(t1), MY(Par(k, t1)), MX(t2), MY(Par(k, t2)));
	    WSetColor (BLUE);
	    WDrawLine(MX(t1), MY(f(t1)), MX(t2), MY(f(t2)));
	    if (diff < fabs(f(t1) - Par(k,t1))) diff = fabs(f(t1) - Par(k,t1));
	}
    }
    WSetColor(RED);
    for (int i = 1; i <= N; i++) WDrawLine(MX(X(i)), h/2-3, MX(X(i)), h/2+3);

    char str[256];
    WSetColor(RED);
    sprintf(str, "Difference: %1.20lf", diff);
    WDrawString(str, 10, 40);
}

int KeyPressFunction (int nKeySym)
{
  switch (nKeySym)
  {
  case XK_Q:
  case XK_q:
  case XK_Escape:
    return KEY_PRESS_QUIT;
  case XK_Return:
    coeff_x = 1.0;
    coeff_y = 1.0;
    break;

  case XK_F1:
    coeff_x *= 2.0;
    break;
  case XK_F2:
    coeff_x *= 0.5;
    break;
  case XK_F3:
    coeff_y *= 2.0;
    break;
  case XK_F4:
    coeff_y *= 0.5;
    break;
  case XK_F5:
    N *=2;
    Interpolate(N);
    break;
  case XK_F6:
    if (N > 8) N /= 2;
    Interpolate(N);
    break;
  case XK_F7:
    disp *=2;
    Interpolate(N);
    break;
  case XK_F8:
    disp *= .5;
    Interpolate(N);
    break;
  default: return KEY_PRESS_NOTHING;
  }

  return KEY_PRESS_EXPOSE;
}

int main ()
{
    printf("Enter number of nodes: ");
    scanf("%d", &N);
    if (N < 3) return 0;

    GetMem(N);
    
//    printf("Enter left edge: ");
//    scanf("%e", &a);
//    printf("Enter right edge: ");
//    scanf("%e", &b);
    a = -1.0;
    b = 1.0;
    if (a >=b) return 0;
    
    Interpolate(N);
    
    printf("Retcode: %d", DrawWindow(DrawWindowContent, KeyPressFunction));
    
    FreeMem();
    return 0;
}
