#include <stdio.h>
#include <math.h>

double alpha = 0.5;

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
  double c = 0.001;

  c *= fabs(alpha) / alpha;

  for (int i = 0; i < 20000; i++) {
    double ddx = dx(x, y);
    double ddy = dy(x, y);
    double len = l(ddx, ddy);
    x += ddx * c;
    y += ddy * c;
    if (l2(x, y) > 200) {
      printf("unstable point! \n");
      return -1;
    }
  }
  // printf("first cycle done\n");
  x_0 = x;
  y_0 = y;

  double time = 0.0;
  double length = 0.0;
  bool returned = false;

  for (int i = 0; i < 12000; i++) {
    double ddx = dx(x, y);
    double ddy = dy(x, y);
    time += fabs(c);
    length += l(ddx * c, ddy * c);

    x += ddx * c;
    y += ddy * c;
    if (l2(x, y) > 100) {
      printf("unstable point! \n");
      return -2;
    }
    if (d(x_0, y_0, x, y) < 1e-2 && i > 100) {
      returned = true;
      printf("Time: $%1.5lf$. Length: $%1.5lf$, Point on the cycle: $(%1.5lf, "
             "%1.5lf)$\n",
             time, length, x, y);
      break;
    }
  }
  if (!returned)
    printf("Trajectory has not returned.\n");

  return 0;
}
