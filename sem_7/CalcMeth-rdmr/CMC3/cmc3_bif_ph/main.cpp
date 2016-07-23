#include <stdio.h>
#include <math.h>

const double alpha = 0.1760127113138164;
// const double alpha = 0.1780127113138164;

const int NN = 20;
const int NNN = 700;
double dx(double x, double y) {
  return y + x * x * x - alpha * x * x * x * x * x;
}
double dy(double x, double y) {
  return -x + y * y * y - alpha * y * y * y * y * y;
}
double l(double a, double b) { return sqrt(a * a + b * b); }
double l2(double a, double b) { return a * a + b * b; }
double d(double x1, double y1, double x2, double y2) {
  return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void draw(double x_0, double y_0) {
  double x = x_0;
  double y = y_0;
  double c = 0.003;
  if (alpha)
    c *= fabs(alpha) / alpha;
  else
    c = -0.003;

  printf("draw (%1.4lf*u, %1.4lf*u)\n", x, y);
  for (int i = 0; i < NNN; i++) {
    double ddx = dx(x, y);
    double ddy = dy(x, y);
    double len = l(ddx, ddy);
    x += ddx * c;
    y += ddy * c;
    if (!(i % NN))
      printf("--(%1.4lf*u, %1.4lf*u)\n", x, y);
    if (l2(x, y) > 300) {
      printf("unstable point! \n");
      return;
    }
  }
  printf("withpen pencircle scaled 0.075pt;\n");
}

int main() {

  double C = 3.3;
  double s = 0.01;
  for (double k = -C; k < C; k += (s * C))
    draw(k, k);
  for (double k = -C; k < C; k += (s * C))
    draw(k, -k);

  for (double k = -C; k < C; k += (s * C))
    draw(k, C);
  for (double k = -C; k < C; k += (s * C))
    draw(k, -C);

  for (double k = -C; k < C; k += (s * C))
    draw(-C, k);
  for (double k = -C; k < C; k += (s * C))
    draw(C, k);
  return 0;
}
