#include <stdio.h>
//#include <conio.h>
#include <math.h>

double alpha = 0.17;

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

int main() {
  double x_0 = 1;
  double y_0 = 1;

  double x = x_0;
  double y = y_0;
  double c = 0.002;
  const int NN = 6;
  const int NNN = 9000;

  c *= fabs(alpha) / alpha;

  printf("draw (%1.4lf*u, %1.4lf*u)\n", x, y);
  for (int i = 0; i < NNN; i++) {
    double ddx = dx(x, y);
    double ddy = dy(x, y);
    double len = l(ddx, ddy);
    x += ddx * c;
    y += ddy * c;
    if (!(i % NN))
      printf("--(%1.4lf*u, %1.4lf*u)\n", x, y);
    if (l2(x, y) > 100) {
      printf("unstable point! \n");
      break;
    }
  }
  //  printf("first cycle done\n");
  x_0 = x;
  y_0 = y;

  double time = 0.0;
  double length = 0.0;

  for (int i = 0; i < NNN; i++) {
    double ddx = dx(x, y);
    double ddy = dy(x, y);
    time += fabs(c);
    length += l(ddx * c, ddy * c);

    x += ddx * c;
    y += ddy * c;
    if (l2(x, y) > 100) {
      printf("unstable point! \n");
      break;
    }
    if (!(i % NN))
      printf("--(%1.4lf*u, %1.4lf*u)\n", x, y);
    if (d(x_0, y_0, x, y) < 1e-2 && i > 100) {
      //     printf("returned, %d.\n", i);
      //    printf("time: %1.5lf, length: %1.5lf, (%1.5lf, %1.5lf)\n", time,
      //    length,x,y);
      break;
    }
  }

  return 0;
}
