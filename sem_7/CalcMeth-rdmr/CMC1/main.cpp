#include <stdio.h>
#include <conio.h>
#include <math.h>

double alpha;
double f(double x) { return ((1 + sin(x)) * sin(x) + 5.0 - alpha) / 3.0; }

double df(double x) {
  double h = 1e-5;
  return (f(x + h) - f(x)) / h;
}

double g(double x) { return (1 + sin(x)) * sin(x) + 5.0 - alpha - 3 * x; }

void graph() {
  double prec = 1e-10;
  double x_0 = 0;
  FILE *gf = fopen("t.mp", "w+");
  for (alpha = -10; alpha < 10; alpha += 0.05) {
    double x = x_0;
    double i = 0;
    double xp = x_0 + 0.1;
    while (true) {
      double new_x = f(x);
      if (fabs(new_x - x) < prec)
        break;
      x = new_x;
      i++;
      if (i > 1000) {
        printf("suxxx\n");
        break;
      }
      // printf("%1.15lf\n", x);
    }
    fprintf(gf, "--(%1.4lf*u, %1.4lf*u)\n", alpha, x);
  }
  fclose(gf);
}

int main() {
  int i = 0;
  double x = 0;
  double prec = 1e-15;
  printf("enter x_0: ");
  double x_0;
  scanf("%lf", &x_0);
  x_0 = 1;
  printf("enter parameter Alpha: ");
  scanf("%lf", &alpha);
  x = x_0;
  if (fabs(df(x)) > 1) {
    printf("unstable solution, try again\n");
    return 0;
  }
  while (true) {
    double new_x = f(x);
    if (fabs(new_x - x) < prec)
      break;
    x = new_x;
    i++;
    // printf("%1.15lf\n", x);
  }
  printf("Parameter: %1.20lf\n", alpha);
  printf("Precision: %1.20lf\n", prec);
  printf("Error: %g\n", g(x));

  printf("Simple iteration: x = %1.20lf, \nIterations: %d\n", x, i);
  i = 0;
  double x1 = -50.0;
  double x2 = 45.0;
  if (g(x1) * g(x2) > 0) {
    printf("wrong starting conditions\n");
    return -1;
  }
  double m;
  while (true) {
    m = (x1 + x2) / 2.0;
    if (g(m) * g(x1) < 0)
      x2 = m;
    else
      x1 = m;
    // printf("%1.15lf\n",m);
    if (fabs(g(m)) < prec)
      break;
    i++;
  }
  printf("Bisection method: x = %1.20lf, \nIterations: %d\n", m, i);
  printf("Error: %g\n", g(m));

  i = 0;
  x = x_0;
  double xp = x_0 + 0.1;
  while (true) {
    double xn = x - g(x) * (x - xp) / (g(x) - g(xp));
    xp = x;
    x = xn;
    if (fabs(g(x)) < prec)
      break;
    i++;
  }
  printf("Newton's method: x = %1.20lf, \nIterations: %d\n", x, i);
  printf("Error: %g\n", g(x));
  getch();
  //	graph();
  //	getch();
  return 0;
}
