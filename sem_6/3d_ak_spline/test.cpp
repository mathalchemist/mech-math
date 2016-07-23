#include <stdio.h>
#include "plot.h"
#include "math.h"
#include "ss.h"
#include "3d.h"

int N = 0;
int M = 0;

double phi = 0;
double psi = 0; // 3.14/6.0;

#define F(i) fv[i - 1]
#define X(i) x[i - 1]
#define D(i) d[i - 1]
#define W(i) w[i - 1]
#define A(i, j) aa[((i - 1 - 1) * (n - 2)) + (j - 1 - 1)]
#define B(i) bb[(i - 1 - 1)]

#define EM(i, j) em[(i - 1) * 4 + (j - 1)]

double em[16];
double exs[4]; // start
double exe[4]; // end
double eb[4];

int method = 0;
int drawint = 0;
int vvv = 0;

double a;
double b;

double *aa = 0;
double *bb = 0;

double *x = 0;
double *w = 0;
double *fv = 0;
double *d = 0;

double coeff_x = 1.0;
double coeff_y = 1.0;

double max = 0.0;
double min = 0.0;

double f(double x, double y) {
  // return x-y;
  return 1.0 / (0.5 + x * x + y * y);
}

double f_x(double x, double y) {
  double h = 0.05;
  return (f(x + h, y) - f(x, y)) / h;
}

double dFxy(double x, double y) {
  double h = 0.05;
  return (f_x(x, y + h) - f_x(x, y)) / h;
}

int MX(double x) {
  return int((x - a) / (b - a) * double(nWWidth) * coeff_x -
             nWWidth / 2 * (coeff_x - 1));
}

int MY(double y) {
  double m = .5 * (max + min);
  double r = coeff_y * fabs(max - min);
  return int((m - y) / r * double(nWHeight) + nWHeight / 2);
}

void FreeMem() {
  delete x;
  delete fv;
  delete w;
  delete d;
  delete aa;
  delete bb;
}

void GetMem(int n) {
  x = new double[n];
  fv = new double[n];
  w = new double[n];
  d = new double[n];
  aa = new double[(n - 2) * (n - 2)];
  bb = new double[n - 2];
}

double FD(int i, int j) { return (F(j) - F(i)) / (X(j) - X(i)); }

void GetMaxMin(double *mx, double *mn) {
  *mx = 0.0;
  *mn = 0.0;

  for (double t = a; t <= b; t += 0.01)
    for (double s = a; s <= b; s += 0.01) {
      double y = f(t, s);
      if (*mx < y)
        *mx = y;
      if (*mn > y)
        *mn = y;
    }
  *mx += 0.1;
  *mn -= 0.1;
}

double Para(int i, double t) {
  double c1 = F(i);
  double c2 = D(i);
  double dx = X(i + 1) - X(i);
  double c3 = (3 * FD(i, i + 1) - 2 * D(i) - D(i + 1)) / dx;
  double c4 = (D(i) + D(i + 1) - 2 * FD(i, i + 1)) / (dx * dx);

  double st = t - X(i);
  return c1 + c2 * st + c3 * st * st + c4 * st * st * st;
}

void GetWeights(int n) {
  for (int i = 2; i <= n - 1; i++)
    W(i) = fabs(FD(i, i + 1) - FD(i - 1, i));
  for (int i = 3; i <= n - 2; i++) {
    if (fabs(W(i + 1) * W(i + 1) + W(i - 1) * W(i - 1)) < 1e-30)
      D(i) = ((X(i + 1) - X(i)) * FD(i - 1, i) +
              (X(i) - X(i - 1)) * FD(i, i + 1)) /
             (X(i + 1) - X(i - 1));
    else
      D(i) = (W(i + 1) * FD(i - 1, i) + W(i - 1) * FD(i, i + 1)) /
             (W(i + 1) + W(i - 1));
  }

  D(1) = FD(1, 2);
  D(2) = FD(2, 3);
  D(n - 1) = FD(n - 2, n - 1);
  D(n) = FD(n - 1, n);
}

void GetNodesAndValues_X(int n, int i) // from formula
{
  double yy = a + (double(i - 1) / double(n - 1)) * (b - a);

  for (int j = 1; j <= n; j++) {
    X(j) = a + (double(j - 1) / double(n - 1)) * (b - a);
    F(j) = f(X(j), yy);
  }
}

void GetNodesAndValues_Y(int n, int i) // X() plays role of Y()
{
  double xx = a + (double(i - 1) / double(n - 1)) * (b - a);

  for (int j = 1; j <= n; j++) {
    X(j) = a + (double(j - 1) / double(n - 1)) * (b - a);
    F(j) = f(xx, X(j));
  }
}

void InterpolateXY(int n) {
  FreeMem();
  GetMem(n);
  return;
}

double Pars(int i, double t) {
  /*    if (i<=3)
      {
          return exs[0]*t*t*t+exs[1]*t*t+exs[2]*t+exs[3];
      }
      if (i>=N-2)
      {
          return exe[0]*t*t*t+exe[1]*t*t+exe[2]*t+exe[3];
      }*/

  double c1 = F(i);
  double c2 = D(i);
  double dx = X(i + 1) - X(i);
  double c3 = (3 * FD(i, i + 1) - 2 * D(i) - D(i + 1)) / dx;
  double c4 = (D(i) + D(i + 1) - 2 * FD(i, i + 1)) / (dx * dx);

  double st = t - X(i);
  return c1 + c2 * st + c3 * st * st + c4 * st * st * st;
}

double Par(int i, double t) {
  if (method == 0)
    return Para(i, t);
  else
    return Pars(i, t);
}

void MakeMatrix(int n) {
  for (int i = 0; i < (n - 2) * (n - 2); i++)
    aa[i] = 0.0;
  for (int i = 0; i < (n - 2); i++)
    bb[i] = 0.0;

  for (int i = 2; i <= n - 1; i++) {
    if (i > 2)
      A(i, i - 1) = X(i + 1) - X(i);
    A(i, i) = 2.0 * (X(i + 1) - X(i - 1));
    if (i < n - 1)
      A(i, i + 1) = X(i) - X(i - 1);
    B(i) = 3.0 * FD(i - 1, i) * (X(i + 1) - X(i)) +
           3.0 * FD(i, i + 1) * (X(i) - X(i - 1));
  }
}

void Interpolatesp(int n) {
  MakeMatrix(n);
  // printf("==================\n");
  //    PrintMatrix(aa,bb,n-2);

  double *ss = new double[n - 2];
  SolveSystem(n - 2, aa, bb, ss);
  for (int i = 2; i < n; i++)
    D(i) = ss[i - 2];
  delete ss;
  D(1) = FD(1, 2);
  D(n) = FD(n - 1, n);

  /*    EM(1,1) = X(1)*X(1)*X(1);
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

      SolveSystem(4,em,eb,exe);*/
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
  double stp = 0.1;
  double lim = -0.000001;

  TVector ZZ(0, 0, 2.0);
  ZZ *= 1.05;
  TVector XX, YY;
  TVector xr, xl, yl, yr;

  TVector zr(0.05, 0.0, -0.1);

  zr += ZZ;
  TVector zl(-0.05, 0.0, -0.1);
  zl += ZZ;

  XX = ZZ;
  XX.RotateY(-3.14 / 2.0);
  YY = ZZ;
  YY.RotateX(3.14 / 2.0);

  xl = zl;
  xl.RotateY(-3.14 / 2.0);
  yl = zl;
  yl.RotateX(3.14 / 2.0);

  xr = zr;
  xr.RotateY(-3.14 / 2.0);
  yr = zr;
  yr.RotateX(3.14 / 2.0);

  zr.RotateZ(phi);
  zr.RotateX(psi);
  zl.RotateZ(phi);
  zl.RotateX(psi);
  yr.RotateZ(phi);
  yr.RotateX(psi);
  yl.RotateZ(phi);
  yl.RotateX(psi);
  xr.RotateZ(phi);
  xr.RotateX(psi);
  xl.RotateZ(phi);
  xl.RotateX(psi);

  ZZ.RotateZ(phi);
  ZZ.RotateX(psi);
  XX.RotateZ(phi);
  XX.RotateX(psi);
  YY.RotateZ(phi);
  YY.RotateX(psi);

  WSetColor(RED);
  // arrows
  WDrawLine(MX(ZZ.Coords[0]), MY(ZZ.Coords[2]), MX(zr.Coords[0]),
            MY(zr.Coords[2]));
  WDrawLine(MX(ZZ.Coords[0]), MY(ZZ.Coords[2]), MX(zl.Coords[0]),
            MY(zl.Coords[2]));
  WDrawLine(MX(YY.Coords[0]), MY(YY.Coords[2]), MX(yr.Coords[0]),
            MY(yr.Coords[2]));
  WDrawLine(MX(YY.Coords[0]), MY(YY.Coords[2]), MX(yl.Coords[0]),
            MY(yl.Coords[2]));
  WDrawLine(MX(XX.Coords[0]), MY(XX.Coords[2]), MX(xr.Coords[0]),
            MY(xr.Coords[2]));
  WDrawLine(MX(XX.Coords[0]), MY(XX.Coords[2]), MX(xl.Coords[0]),
            MY(xl.Coords[2]));
  // axes
  WDrawLine(MX(0), MY(0), MX(ZZ.Coords[0]), MY(ZZ.Coords[2]));
  WDrawLine(MX(0), MY(0), MX(XX.Coords[0]), MY(XX.Coords[2]));
  WDrawLine(MX(0), MY(0), MX(YY.Coords[0]), MY(YY.Coords[2]));

  for (int i = 1; i < N; i++) // cycle throgh Y
  {
    GetNodesAndValues_X(n, i);

    if (drawint == 0) {
      if (method == 0)
        GetWeights(n);
      else
        Interpolatesp(n);
    }

    for (int j = 1; j < N; j++) // X
    {
      TVector p11(X(j), X(i), f(X(j), X(i)));
      TVector p12(X(j + 1), X(i), f(X(j + 1), X(i)));
      TVector p21(X(j), X(i + 1), f(X(j), X(i + 1)));
      TVector p22(X(j + 1), X(i + 1), f(X(j + 1), X(i + 1)));

      p11.RotateZ(phi);
      p11.RotateX(psi);
      p12.RotateZ(phi);
      p12.RotateX(psi);
      p21.RotateZ(phi);
      p21.RotateX(psi);
      p22.RotateZ(phi);
      p22.RotateX(psi);

      double sx11 = p11.Coords[0];
      double sy11 = p11.Coords[2];
      double sx12 = p12.Coords[0];
      double sy12 = p12.Coords[2];
      double sx21 = p21.Coords[0];
      double sy21 = p21.Coords[2];
      double sx22 = p22.Coords[0];
      double sy22 = p22.Coords[2];

      bool v = true;

      TVector norm11 = Grad(p11.Coords[0], p11.Coords[1]);
      TVector norm12 = Grad(p12.Coords[0], p12.Coords[1]);
      TVector norm21 = Grad(p21.Coords[0], p21.Coords[1]);
      TVector norm22 = Grad(p22.Coords[0], p22.Coords[1]);
      norm11.RotateZ(phi);
      norm12.RotateX(psi);
      norm21.RotateZ(phi);
      norm22.RotateX(psi);

      if (norm11.Coords[1] < lim && norm12.Coords[1] < lim &&
          norm21.Coords[1] < lim && norm22.Coords[1] < lim)
        v = false;
      if (vvv == 1)
        v = true;

      if (v) {
        TVector n = Grad(p11.Coords[0], p11.Coords[1]);
        TVector S(0, 1.0, 0);
        double Cos = fabs(n | S);
        int ic = int(255.0 * Cos);

        //		int dwc = (ic << (8+8)) + (ic << 8) + ic;
        int dwc = (ic / 8 << (5 + 6)) + (ic / 4 << 5) + ic / 8;

        WSetColor(dwc);

        WDrawLine(MX(sx11), MY(sy11), MX(sx12), MY(sy12));
        WDrawLine(MX(sx12), MY(sy12), MX(sx22), MY(sy22));
        WDrawLine(MX(sx22), MY(sy22), MX(sx21), MY(sy21));
        WDrawLine(MX(sx21), MY(sy21), MX(sx11), MY(sy11));

        if (drawint == 0) {
          if (method == 0)
            WSetColor(GREEN);
          else
            WSetColor(YELLOW);
          for (double t = X(j); t < X(j + 1); t += stp) {
            TVector pp1(t, X(i), Par(j, t));
            TVector pp2(t + stp, X(i), Par(j, t + stp));
            pp1.RotateZ(phi);
            pp1.RotateX(psi);
            pp2.RotateZ(phi);
            pp2.RotateX(psi);
            double six1 = pp1.Coords[0];
            double siy1 = pp1.Coords[2];
            double six2 = pp2.Coords[0];
            double siy2 = pp2.Coords[2];
            WDrawLine(MX(six1), MY(siy1), MX(six2), MY(siy2));
          }
        }
      }
    }
  }

  for (int i = 1; i < N; i++) // cycle throgh Y
  {
    GetNodesAndValues_Y(n, i);
    if (drawint == 0) {
      if (method == 0)
        GetWeights(n);
      else
        Interpolatesp(n);
    }

    for (int j = 1; j < N; j++) // X
    {
      TVector p11(X(i), X(j), f(X(i), X(j)));
      TVector n = Grad(p11.Coords[0], p11.Coords[1]);
      n.RotateZ(phi);
      n.RotateX(psi);
      bool v = true;
      if (n.Coords[1] < lim)
        v = false;
      ;
      if (vvv == 1)
        v = true;

      if (v) {
        if (drawint == 0) {
          if (method == 0)
            WSetColor(GREEN);
          else
            WSetColor(YELLOW);

          for (double t = X(j); t < X(j + 1); t += stp) {
            TVector pp1(X(i), t, Par(j, t));
            TVector pp2(X(i), t + stp, Par(j, t + stp));
            pp1.RotateZ(phi);
            pp1.RotateX(psi);
            pp2.RotateZ(phi);
            pp2.RotateX(psi);
            double six1 = pp1.Coords[0];
            double siy1 = pp1.Coords[2];
            double six2 = pp2.Coords[0];
            double siy2 = pp2.Coords[2];
            WDrawLine(MX(six1), MY(siy1), MX(six2), MY(siy2));
          }
        }
      }
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
    method = (method + 1) % 2;
    break;

  case XK_BackSpace:
    vvv = (vvv + 1) % 2;
    break;

  case XK_F:
  case XK_f:
    drawint = (drawint + 1) % 2;
    break;

  case XK_F1:
    coeff_x *= 1.2;
    break;
  case XK_F2:
    coeff_x /= 1.2;
    break;

  case XK_F3:
    coeff_y *= 1.2;
    break;
  case XK_F4:
    coeff_y /= 1.2;
    break;

  case XK_a:
    N *= 2;
    InterpolateXY(N);
    break;
  case XK_z:
    if (N > 4)
      N /= 2;
    InterpolateXY(N);
    break;

  case XK_F7:
    if (phi < 3.14 / 3.0)
      phi += 0.025;
    break;
  case XK_F8:
    if (phi > -3.14 / 3.0)
      phi -= 0.025;
    break;

  case XK_F5:
    psi += 0.05;
    break;
  case XK_F6:
    psi -= 0.05;
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

  a = -3;
  b = 3;
  if (a >= b)
    return 0;

  GetMem(N);
  GetMaxMin(&max, &min);
  InterpolateXY(N);

  printf("Retcode: %d", DrawWindow(DrawWindowContent, KeyPressFunction));

  FreeMem();
  return 0;
}
