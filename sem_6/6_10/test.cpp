#include <stdio.h>
#include "plot_x11.h"
#include "math.h"

const double h = 0.00001;
const double IStep = 0.000001;

// interpolation
int N = 0;
double M; // norm of difference
double a;
double b;
double *x;

double f(double x) { return sqrt(fabs(x)); }
double f__(double x) { return (f(x + h) - 2 * f(x) + f(x - h)) / (h * h); }

double F(double t) {
  if (t < x[0] || t > x[N])
    return 0;
  for (int i = 0; i < N; i++)
    if (x[i] <= t && t <= x[i + 1])
      return f(x[i]) + (t - x[i]) * (f(x[i + 1]) - f(x[i])) / (x[i + 1] - x[i]);
  return 0;
}

double Int(double a, double b) {
  double S = 0;
  for (double t = a; t < b; t += IStep)
    S += sqrt(fabs(f__(t))) * IStep;
  return S;
}

int CalcNodes() {
  double step = 0.0001 / double(N);
  double t = a;
  int i = 0;
  double FInt = Int(a, b);
  printf("FInt = %1.5lf\n", FInt);
  double TInt = 0;
  while (t <= b + step) {
    t += step;
    TInt += step * sqrt(fabs(f__(t)));
    //	printf("f''(%1.5lf) = %1.5lf; TInt = %1.5lf\n", t, f__(t), TInt);
    if (fabs(double(i - 1) / double(N - 1) * FInt - TInt) < .25) {
      x[i] = t;
      //	    printf("Node x[%d] = %1.5lf\n", i, x[i]);
      i++;
    }
  }
  x[i] = b; // x_n = b
  return i;
}

/* double SX(int i) { return a + double(i/nWWidth)*(b-a); } double SY(int i) {
 * return 1.0 - double(i)/double(nWWidth); } */

int MX(double x) { return int((x - a) / (b - a) * double(nWWidth)); }
int MY(double y) { return int((1.0 - y) * nWHeight); }

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
  WDrawString("Press Q to quit, F1...F4 to change function", 10, 20);

  WSetColor(GREEN);
  for (int i = 0; i < N; i++)
    WDrawLine(MX(x[i]), MY(f(x[i])), MX(x[i + 1]), MY(f(x[i + 1])));
  WSetColor(BLUE);
  double stp = 0.0001;
  for (double x = a; x < b; x += stp)
    WDrawLine(MX(x), MY(f(x)), MX(x + stp), MY(f(x + stp)));
  //    WSetColor (RED);
  //    for (double x = a; x < b; x+=stp)
  //	WDrawLine(MX(x), MY(F(x)), MX(x+stp), MY(F(x+stp)));
}

int KeyPressFunction(int nKeySym) {
  switch (nKeySym) {
  case XK_Q:
  case XK_q:
    return KEY_PRESS_QUIT;

  /*  case XK_F1:
    case XK_1:
      variant = 1;
      WSetTitle ("Function 1");
      break;
    case XK_F2:
    case XK_2:
      variant = 2;
      WSetTitle ("Function 2");
      break;
    case XK_F3:
    case XK_3:
      variant = 3;
      WSetTitle ("Function 3");
      break;
    case XK_F4:
    case XK_4:
      variant = 4;
      WSetTitle ("Function 4");
      break;*/

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

  x = new double[2 * N];

  //    printf("Enter left edge: ");
  //    scanf("%e", &a);
  //    printf("Enter right edge: ");
  //    scanf("%e", &b);
  a = -1.0;
  b = 1.0;
  if (a >= b)
    return 0;

  N = CalcNodes();

  // printf("real nodes q: %d\n", N);

  //    printf("Emergency exit!\n");
  //    return 0;
  double sttp = 0.01;
  for (double t = a + 0.05; t <= b - 0.05; t += sttp) {
    M += sttp * (fabs(f(t) - F(t)));
  }
  printf("Diff = %1.10lf\n", M);

  printf("Retcode: %d", DrawWindow(DrawWindowContent, KeyPressFunction));
  delete x;
  return 0;
}
