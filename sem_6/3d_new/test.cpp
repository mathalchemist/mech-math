#include <stdio.h>
#include "plot_x11.h"
#include "math.h"
#include "3d.h"

// interpolation
int N = 0;
double a;
double b;

double ph = 0;           // angle \varphi
double ps = 3.14 / 12.0; // angle \psi

double coeff_x = 1.0;
double coeff_y = 1.0;

double SOMEWHERE_FAR_BEYOND = 50.0;

TVector C(0, -10.0, 1.0);
TVector S = C * .75;

TVector O(0, 0, 0);

double max = 0;
double min = 0;

double *x = 0;
double *fv = 0;
double *xi = 0;
double *v = 0;

double *c1 = 0;
double *c2 = 0;
double *c3 = 0;

double *aa = 0;
double *bb = 0;

#define A(i, j) aa[((i - 1) * (n + 1)) + (j - 1)]
#define F(i) fv[i - 1]
#define X(i) x[i - 1]
#define XI(i) xi[i - 1]
#define B(i) bb[i - 1]
#define V(i) v[i - 1]

double f(double x, double y) { return 1.0 / (1.0 + x * x + y * y); }

int MX(double x) {
  return int((x - a) / (b - a) * double(nWWidth) * coeff_x -
             nWWidth / 2 * (coeff_x - 1.0));
}

int MY(double y) {
  double m = .5 * (max + min);
  double r = coeff_y;
  return int(r * (m - y) / (max - min) * double(nWHeight) + nWHeight / 2);
}

void pv(TVector x) {
  printf("(%1.5lf, %1.5lf, %1.5lf)\n", x.Coords[0], x.Coords[1], x.Coords[2]);
}

void getp(int i, int j, TVector *p11, TVector *p12, TVector *p21,
          TVector *p22) {
  double xx;
  double yy;
  double ii = double(i);
  double jj = double(j);
  double NN = double(N);

  xx = a + (b - a) * ii / NN;
  yy = a + (b - a) * jj / NN;

  *p11 = TVector(xx, yy, f(xx, yy));

  xx = a + (b - a) * ii / NN;
  yy = a + (b - a) * (jj + 1.0) / NN;
  *p12 = TVector(xx, yy, f(xx, yy));

  xx = a + (b - a) * (ii + 1.0) / NN;
  yy = a + (b - a) * jj / NN;
  *p21 = TVector(xx, yy, f(xx, yy));

  xx = a + (b - a) * (ii + 1.0) / NN;
  yy = a + (b - a) * (jj + 1.0) / NN;
  *p22 = TVector(xx, yy, f(xx, yy));
}

TVector Project(TVector p, TVector Cr, TVector Sr) {
  TVector V((p - Cr).Normalize());
  TVector N((Cr - O).Normalize());
  double t = ((Sr - Cr) | N) / (V | N);

  TVector Ps(Cr + V * t);
  Ps.RotateZ(ph);
  return Ps;
}

bool CheckVis(TVector P, TVector Cr, TVector Sr) {
  TVector V((P - Cr).Normalize());
  TVector N((Cr - O).Normalize());
  double t = ((Sr - Cr) | N) / (V | N);

  //    if (t < 0.0) printf("< 0\n");

  bool vis = false;
  //    printf("SFB: %2.5lf\n", SOMEWHERE_FAR_BEYOND);

  for (double s = .25 * t; s < SOMEWHERE_FAR_BEYOND; s += 0.005) {
    TVector pp;
    pp = Cr + V * s;
    double ppx = pp.Coords[0];
    double ppy = pp.Coords[1];
    TVector ff(ppx, ppy, f(ppx, ppy));
    double pdist = (pp - P).Length();
    double fdist = (pp - ff).Length();
    if (pdist < 0.02) {
      vis = true;
      break;
    }
    if (fdist < 0.015 && pdist > .2) {
      vis = false;
      break;
    }
  }
  return vis;
}

TVector Grad(double x, double y) {
  double h = 0.05;
  TVector v0(x, y, f(x, y));
  TVector v1(x + h, y, f(x + h, y));
  TVector v2(x, y + h, f(x, y + h));
  return ((v1 - v0) & (v2 - v0)).Normalize();
}

void DrawWindowContent() {
  int w = nWWidth;
  int h = nWHeight;

  WSetColor(BLUE);
  WFillRectangle(0, 0, w, h);
  WSetColor(BLACK);

  int n = N;

  for (int i = 0; i < N; i++) {
    FreeMem();
    GetMem(n);
    GetNodesAndValues_Y(N, i);
    MakeMatrix(n);
    SolveSystem(n + 1, aa, bb, v);
    for (int j = 1; j <= N; j++)
      GetC_i(j);

    for (int j = 0; j < N; j++) {
      TVector p11;
      TVector p12;
      TVector p21;
      TVector p22;

      getp(i, j, &p11, &p12, &p21, &p22);
      TVector ip11 = (p11 + p12) * .5;
      TVector ip12 = (p12 + p22) * .5;
      TVector ip21 = (p22 + p21) * .5;
      TVector ip22 = (p21 + p11) * .5;

      TVector Cr(C);
      TVector Sr(S);

      Cr.RotateZ(-ph); // negative!
      Sr.RotateZ(-ph);

      TVector P1 = (p11 + p12 + p21) / 3.0;
      TVector P2 = (p12 + p21 + p22) / 3.0;

      bool v1 = CheckVis(P1, Cr, Sr);
      bool v2 = CheckVis(P2, Cr, Sr);

      TVector ips11 = Project(ip11, Cr, Sr);
      TVector ips12 = Project(ip12, Cr, Sr);
      TVector ips21 = Project(ip21, Cr, Sr);
      TVector ips22 = Project(ip22, Cr, Sr);

      TVector ps11 = Project(p11, Cr, Sr);
      TVector ps12 = Project(p12, Cr, Sr);
      TVector ps21 = Project(p21, Cr, Sr);
      TVector ps22 = Project(p22, Cr, Sr);

      double sx11 = ps11.Coords[0];
      double sy11 = ps11.Coords[2];
      double sx12 = ps12.Coords[0];
      double sy12 = ps12.Coords[2];
      double sx21 = ps21.Coords[0];
      double sy21 = ps21.Coords[2];
      double sx22 = ps22.Coords[0];
      double sy22 = ps22.Coords[2];

      double isx11 = ips11.Coords[0];
      double isy11 = ips11.Coords[2];
      double isx12 = ips12.Coords[0];
      double isy12 = ips12.Coords[2];
      double isx21 = ips21.Coords[0];
      double isy21 = ips21.Coords[2];
      double isx22 = ips22.Coords[0];
      double isy22 = ips22.Coords[2];

      if (v1) {

        TVector n = Grad(P1.Coords[0], P1.Coords[1]);
        TVector v = (P1 - Cr).Normalize();
        double Cos = fabs(n | v);
        int ic = int(255.0 * Cos);
        WSetColor((ic << (8 + 8)) + (ic << 8) + ic);

        WDrawLine(MX(sx11), MY(sy11), MX(sx12), MY(sy12));
        WDrawLine(MX(sx12), MY(sy12), MX(sx21), MY(sy21));
        WDrawLine(MX(sx21), MY(sy21), MX(sx11), MY(sy11));
      }
      if (v2) {

        TVector n = Grad(P2.Coords[0], P2.Coords[1]);
        TVector v = (P2 - Cr).Normalize();
        double Cos = fabs(n | v);

        int ic = int(255.0 * Cos);
        WSetColor((ic << (8 + 8)) + (ic << 8) + ic);

        WDrawLine(MX(sx12), MY(sy12), MX(sx22), MY(sy22));
        WDrawLine(MX(sx22), MY(sy22), MX(sx21), MY(sy21));
        WDrawLine(MX(sx21), MY(sy21), MX(sx12), MY(sy12));
      }
    }
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
      coeff_x *= 1.3;
      break;
    case XK_F2:
      coeff_x /= 1.3;
      break;

    case XK_F3:
      coeff_y *= 1.3;
      break;
    case XK_F4:
      coeff_y /= 1.3;
      break;

    case XK_F5:
      N *= 2;
      break;
    case XK_F6:
      if (N > 4)
        N /= 2;
      break;

    case XK_F7:
      ph += 0.05;
      break;
    case XK_F8:
      ph -= 0.05;
      break;

    case XK_A:
    case XK_a:
      C += TVector(0, 0, .1);
      break;
    case XK_Z:
    case XK_z:
      C -= TVector(0, 0, .1);
      break;

    case XK_D:
    case XK_d:
      O += TVector(0, 0, .1);
      break;
    case XK_C:
    case XK_c:
      O -= TVector(0, 0, .1);
      break;

    case XK_F:
    case XK_f:
      SOMEWHERE_FAR_BEYOND += 1.0;
      break;
    case XK_V:
    case XK_v:
      SOMEWHERE_FAR_BEYOND -= 1.0;
      break;

    case XK_S:
    case XK_s:
      S *= 1.1;
      break;
    case XK_X:
    case XK_x:
      S /= 1.1;
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

    a = -3.5;
    b = 3.5;
    if (a >= b)
      return 0;

    for (double t = a; t <= b; t += 0.1)
      for (double s = a; s <= b; s += 0.1) {
        double v = f(t, s);
        if (max < v)
          max = v;
        if (min > v)
          min = v;
      }
    max += 0.05;
    min -= 0.05;

    O.Coords[2] = .5 * (max + min);

    printf("Retcode: %d", DrawWindow(DrawWindowContent, KeyPressFunction));
    return 0;
  }

  // interpolation

  void GetMem(int n, double *x, double *fv, double *xi, double *aa, double *b,
              double *v) {
    x = new double[n];
    fv = new double[n];
    xi = new double[n + 1];
    v = new double[n + 1];
    aa = new double[(n + 1) * (n + 1)];
    bb = new double[n + 1];
    c1 = new double[n * n];
    c2 = new double[n * n];
    c3 = new double[n * n];
  }

  void FreeMem(double *x, double *fv, double *xi, double *aa, double *b) {
    delete x;
    delete fv;
    delete xi;
    delete aa;
    delete bb;
    delete v;
    delete c1;
    delete c2;
    delete c3;
  }

  void GetC_i(int i) {
    double fr =
        (V(i + 1) - F(i)) / (XI(i + 1) - X(i)) - (F(i) - V(i)) / (X(i) - XI(i));

    C1(i) = V(i);
    C2(i) = (F(i) - V(i)) / (X(i) - XI(i)) -
            (X(i) - XI(i)) / (XI(i + 1) - XI(i)) * fr;
    C3(i) = 1.0 / (XI(i + 1) - XI(i)) * fr;
  }

  double Par(int i, double t) {
    double d = t - XI(i);
    return C1(i) + C2(i) * d + C3(i) * d * d;
  }

  void GetNodesAndValues_X(int n, int j) {
    double y = a + (double(j) / double(n)) * (b - a);
    for (int i = 1; i <= n; i++) {
      X(i) = a + (double(i) / double(n)) * (b - a);
      F(i) = f(X(i), y);
    }
    for (int i = 2; i <= n; i++)
      XI(i) = .5 * (X(i - 1) + X(i));
    XI(1) = a - (XI(2) - a);
    XI(n + 1) = b + (b - XI(n));
  }

  void GetNodesAndValues_Y(int n, int i) {
    double x = a + (double(i) / double(n)) * (b - a);
    for (int i = 1; i <= n; i++) {
      X(i) = a + (double(i) / double(n)) * (b - a);
      F(i) = f(x, X(i));
    }
    for (int i = 2; i <= n; i++)
      XI(i) = .5 * (X(i - 1) + X(i));
    XI(1) = a - (XI(2) - a);
    XI(n + 1) = b + (b - XI(n));
  }

  void MakeMatrix(int n) {
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
  }

  FreeMem();
  GetMem(n);

  GetNodesAndValues(n);
  MakeMatrix(n);
  SolveSystem(n + 1, aa, bb, v);

  // X ==========
  for (int j = 1; j < N; j++) {
    GetNodesAndValues_X(n, j);
    MakeMatrix(n);
    SolveSystem(n + 1, aa, bb, v);
    for (int i = 1; i <= N; i++)
      GetC_i(i);
  }
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
  WDrawString("Q=quit, F1..F4 -- scale, F5/F6 -- node count, F7/F8 -- add d(x)",
              10, 20);

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
      double dd = fabs(f(t1) - Par(i, t1));
      if (diff < dd)
        diff = dd;
    }
  }
  WSetColor(RED);
  for (int i = 1; i < N + 1; i++)
    WDrawLine(MX(X(i)), h / 2 - 3, MX(X(i)), h / 2 + 3);

  char str[256];
  WSetColor(RED);
  sprintf(str, "Difference: %1.20lf", diff);
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

  GetMem(N);

  //    printf("Enter left edge: ");
  //    scanf("%e", &a);
  //    printf("Enter right edge: ");
  //    scanf("%e", &b);
  a = -1.0;
  b = 1.0;
  if (a >= b)
    return 0;

  Interpolate(N);

  printf("Retcode: %d", DrawWindow(DrawWindowContent, KeyPressFunction));
  FreeMem();
  return 0;
}
