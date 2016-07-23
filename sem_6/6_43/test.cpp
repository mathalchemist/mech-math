#include <stdio.h>
#include "plot_x11.h"
#include "math.h"
#include "ss.h"

#define A(i, j) aa[((i - 1) * (n + 1)) + (j - 1)]
#define F(i) fv[i - 1]
#define X(i) x[i - 1]
#define XI(i) xi[i - 1]
#define B(i) bb[i - 1]
#define V(i) v[i - 1]

// interpolation
int N = 0;
double a;
double b;

double coeff_y = 1;
double coeff_x = 1;

double max = 0;
double min = 0;

double *x = 0;
double *fv = 0;
double *xi = 0;
double *v = 0;

double *aa = 0;
double *bb = 0;

double disp = 1;

double delta(double x, double a) { return exp(-(x - a) * (x - a) / disp); }

double f(double x) {
  return sqrt(fabs(x)) + .25 * delta(x, 0.5);
  //    return sin(x)*x;
}

double Par(int i, double t) {
  double fr =
      (V(i + 1) - F(i)) / (XI(i + 1) - X(i)) - (F(i) - V(i)) / (X(i) - XI(i));

  double c1 = V(i);
  double c2 = (F(i) - V(i)) / (X(i) - XI(i)) -
              (X(i) - XI(i)) / (XI(i + 1) - XI(i)) * fr;
  double c3 = 1.0 / (XI(i + 1) - XI(i)) * fr;
  double d = t - XI(i);
  return c1 + c2 * d + c3 * d * d;
}

void GetNodesAndValues(int n) // from formula
{
  max = 0;
  min = 0;

  for (int i = 1; i <= n; i++) {
    X(i) = a + (double(i) / double(n)) * (b - a);
    F(i) = f(X(i));
    if (max < F(i))
      max = F(i);
    if (min > F(i))
      min = F(i);
  }
  for (int i = 2; i <= n; i++)
    XI(i) = .5 * (X(i - 1) + X(i));
  XI(1) = a - (XI(2) - a);
  XI(n + 1) = b + (b - XI(n));
}

void Interpolate(int n) {
  delete x;
  delete fv;
  delete xi;
  delete v;
  delete aa;
  delete bb;
  x = new double[n];
  fv = new double[n];
  xi = new double[n + 1];
  v = new double[n + 1];
  aa = new double[(n + 1) * (n + 1)];
  bb = new double[n + 1];

  GetNodesAndValues(n);

  for (int i = 1; i <= n + 1; i++)
    for (int j = 1; j <= n + 1; j++)
      A(i, j) = 0;

  for (int i = 2; i <= n; i++) // make system
  {
    A(i, i - 1) = 1.0 / (X(i - 1) - XI(i - 1)) - 1 / (XI(i) - XI(i - 1));
    A(i, i) = 1.0 / (XI(i) - X(i - 1)) + 1.0 / (XI(i) - XI(i - 1)) +
              1.0 / (X(i) - XI(i)) + 1.0 / (XI(i + 1) - XI(i));
    A(i, i + 1) = 1.0 / (XI(i + 1) - X(i)) - 1.0 / (XI(i + 1) - XI(i));

    B(i) =
        F(i - 1) * (1.0 / (X(i - 1) - XI(i - 1)) + 1.0 / (XI(i) - X(i - 1))) +
        F(i) * (1.0 / (X(i) - XI(i)) + 1.0 / (XI(i + 1) - X(i)));
  }

  // border conditions
  int i;
  i = 1;
  A(i, i) = 1.0 / (X(i) - XI(i));
  A(i, i + 1) = 1.0 / (XI(i + 1) - X(i));
  B(i) = F(i) * (1.0 / (X(i) - XI(i)) + 1.0 / (XI(i + 1) - X(i)));

  i = n;
  A(i + 1, i) = 1.0 / (X(i) - XI(i));
  A(i + 1, i + 1) = 1.0 / (XI(i + 1) - X(i));
  B(i + 1) = F(i) * (1.0 / (X(i) - XI(i)) + 1.0 / (XI(i + 1) - X(i)));

  //    PrintMatrix(aa,bb,n+1);

  SolveSystem(n + 1, aa, bb, v);
}

/* double SX(int i) { return a + double(i/nWWidth)*(b-a); } double SY(int i) {
 * return 1.0 - double(i)/double(nWWidth); } */

int MX(double x) {
  return int((x - a) / (b - a) * double(nWWidth) * coeff_x -
             nWWidth / 2 * (coeff_x - 1));
}

int MY(double y) {
  double m = .5 * (max + min);
  double r = coeff_y * fabs(max - min);
  return int((m - y) / r * double(nWHeight) + nWHeight / 2);
}

void DrawWindowContent() {
  int w = nWWidth;
  int h = nWHeight;

  WSetColor(DARKGRAY);
  WFillRectangle(0, 0, w, h);
  WSetColor(BLACK);
  // axis
  WDrawLine(0, h / 2, w, h / 2);
  WDrawLine(w / 2, 0, w / 2, h);
  // arrows
  WDrawLine(w / 2, 0, w / 2 - 5, 5);
  WDrawLine(w / 2, 0, w / 2 + 5, 5);

  WDrawLine(w, h / 2, w - 5, h / 2 - 5);
  WDrawLine(w, h / 2, w - 5, h / 2 + 5);
  WSetColor(RED);
  WDrawString("Q=quit, F1..F4 -- change scale, F5/F6 -- change node count", 10,
              20);

  double diff = 0;
  for (int i = 1; i < N + 1; i++) {
    int ss = int((XI(i) - a) / (b - a) * w);
    int se = int((XI(i + 1) - a) / (b - a) * w);
    for (int j = ss; j <= se; j++) {
      double t1 = (double(j) / double(w)) * (b - a) + a;
      double t2 = (double(j + 1) / double(w)) * (b - a) + a;
      WSetColor(GREEN);

      WDrawLine(MX(t1), MY(Par(i, t1)), MX(t2), MY(Par(i, t2)));
      WSetColor(BLUE);
      WDrawLine(MX(t1), MY(f(t1)), MX(t2), MY(f(t2)));
      if (diff < fabs(f(t1) - Par(i, t1)))
        diff = fabs(f(t1) - Par(i, t1));
    }
  }
  WSetColor(RED);
  for (int i = 1; i < N + 1; i++)
    WDrawLine(MX(X(i)), h / 2 - 3, MX(X(i)), h / 2 + 3);

  char str[256];
  WSetColor(RED);
  sprintf(str, "Difference: %1.10lf", diff);
  WDrawString(str, 10, 40);
}

int KeyPressFunction(int nKeySym) {
  switch (nKeySym) {
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
    N *= 2;
    Interpolate(N);
    break;
  case XK_F6:
    if (N > 4)
      N /= 2;
    Interpolate(N);
    break;
  case XK_F7:
    disp *= 2;
    Interpolate(N);
    break;
  case XK_F8:
    disp *= .5;
    Interpolate(N);
    break;
  default:
    return KEY_PRESS_NOTHING;
  }

  return KEY_PRESS_EXPOSE;
}

int main() {
  printf("Enter number of nodes: ");
  scanf("%d", &N);
  if (N < 2)
    return 0;

  x = new double[N];

  //    printf("Enter left edge: ");
  //    scanf("%e", &a);
  //    printf("Enter right edge: ");
  //    scanf("%e", &b);
  a = -1.0;
  b = 1.0;
  if (a >= b)
    return 0;

  Interpolate(N);

  //    double sttp = 0.01;
  //    for (double t = a+0.05; t <=b-0.05; t+= sttp) { M += sttp * (fabs(f(t) -
  //    Par(t))); }
  //    printf("Diff = %1.10lf\n", M);

  printf("Retcode: %d", DrawWindow(DrawWindowContent, KeyPressFunction));
  delete x;
  delete fv;
  delete xi;
  delete v;
  delete aa;
  delete bb;
  return 0;
}
