#include <stdio.h>
#include "plot_x11.h"
#include "math.h"

#define F(i) fv[i-1]
#define X(i) x[i-1]
#define D(i) d[i-1]
#define W(i) w[i-1]

// interpolation
int N = 0;
double a;
double b;

double coeff_y = 1;
double coeff_x = 1;

double max = 0;
double min = 0;

double * x=0;
double * w=0;
double * fv = 0;
double * d = 0;

double f(double x)
{
    return 1.0/(1.0+x*x);
//    sqrt(fabs(x))
//sin(4.0*x) + .25*delta(x,0.5);
//    return sin(x)*x;
}

double disp = 1.0;

double delta(double x, double a)
{
    return exp(-(x-a)*(x-a)/disp);
}


void FreeMem()
{
    delete x;
    delete fv;
    delete d;
    delete w;
}
void GetMem(int n)
{
    x = new double[n];
    fv = new double[n];
    d = new double[n];
    w = new double[n];
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
    double c1 = F(i);
    double c2 = D(i);
    double dx = X(i+1)-X(i);
    double c3 = (3*FD(i,i+1) -2*D(i)-D(i+1))/dx;
    double c4 = (D(i) + D(i+1)-2*FD(i,i+1))/(dx*dx);
    
    double st = t - X(i);
    return c1 + c2 * st + c3 * st*st + c4 * st*st*st;
}

void GetWeights(int n)
{
    for (int i = 2; i <= n-1; i++) W(i) = fabs(FD(i,i+1)-FD(i-1,i));
    D(1) = FD(1,2);
    D(2) = FD(2,3);
    for (int i = 3; i <= n-2; i++)
    {
	if (fabs(W(i+1)*W(i+1) + W(i-1)*W(i-1)) < 1e-30)
	    D(i) = ((X(i+1)-X(i))*FD(i-1,i)+(X(i)-X(i-1))*FD(i,i+1))/(X(i+1)-X(i-1));
	else
	    D(i) = (W(i+1)*FD(i-1,i)+W(i-1)*FD(i,i+1))/(W(i+1)+W(i-1));
    }
    D(n-1) = FD(n-2,n-1);
    D(n) = FD(n-1,n);
}

void GetNodesAndValues(int n) // from formula
{
    for (int i = 1; i <= n; i++)
    {
     	X(i) = a + (double(i-1)/double(n-1)) * (b-a);
	F(i) = f(X(i));
    }
}

void Interpolate(int n)
{
    FreeMem();
    GetMem(n);

    GetMaxMin();
    GetNodesAndValues(n);
    GetWeights(n);
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
    if (N > 6) N /= 2;
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
    delete x;
    delete fv;
    delete d;
    delete w;
    return 0;
}
