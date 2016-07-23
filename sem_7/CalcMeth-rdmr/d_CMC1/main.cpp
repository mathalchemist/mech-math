#include <stdio.h>
#include <conio.h>
#include <math.h>

const int MAX_ITER = 10000;

const double PI = 4 * atan(1.0);

double alpha;

double f(double x) {
  double xs = (x - alpha / 2) * (x - alpha);
  if (xs < 0)
    return -1000.0;
  else
    return sqrt(xs);
  // 2.0/3.0 *(x*x+0.5-1.0/alpha*sin(alpha*x)/alpha*(sin(alpha*x)));
}

double df(double x) {
  // 4.0/3.0*x - 1.0*sin(2*alpha*x)/alpha;
  double h = 1e-14;
  return (f(x + h) - f(x)) / h;
}

double g(double x) { return f(x) - sin(x); }

/*void graph()
{
        double prec = 1e-10;
        double x_0 = 0;
        FILE * gf = fopen("t.mp", "w+");
        for (alpha = -10; alpha < 10; alpha+=0.05)
        {
                double x = x_0;
                double i = 0;
                double xp = x_0+0.1;
                while (true)
                {
                        double new_x = f(x);
                        if (fabs(new_x-x) < prec) break;
                        x = new_x;
                        i++;
                        if (i >1000) { printf("suxxx\n"); break; }
                        //printf("%1.15lf\n", x);
                }
                fprintf(gf, "--(%1.4lf*u, %1.4lf*u)\n", alpha,x);
        }
        fclose(gf);
}
*/

int method_iteration(double prec, double mu_zoom) {
  int iter = 0;
  int pre_iter = 0;
  int wave_num;

  // --------------------
  // alpha+ root tracking
  // --------------------
  wave_num = int(floor(alpha / PI));
  if (wave_num % 2) {
    printf("Droite racine ne pas exister!\n");
  } else {
    double x_0 = (wave_num + 1) * PI;
    printf("Le x_0 initale (mod Pi):       %1.20lf\n", x_0 / PI);
    double x_left = x_0;
    while (g(x_left) >= 0) {
      x_left = .5 * (alpha + x_left);
      pre_iter++;
    }
    printf("Le x_left finale (mod Pi):     %1.20lf\n", x_left / PI);
    // now x_left is the "first" point where g < 0.
    // double mu_zoom;
    // printf("Entrez de parametre MU_ZOOM: ");
    // scanf("%lf", &mu_zoom);
    double mu = mu_zoom / df(x_left);
    x_0 = 2 * x_left - alpha;
    printf("Le x_0 finale (mod Pi):        %1.20lf\n", x_0 / PI);
    double x = x_0;
    printf("Atteindre le fidelite et resolution via mu = %1.20lf!\n", mu);
    while (true) {
      double new_x = x - mu * g(x);
      if (f(new_x) < 0) {
        printf("ERREURE DE FATALE: Methode ne pas convergente!\n");
        break;
      }
      if (fabs(g(x)) < prec) {
        printf("Atteindre le pres! Voila!\n");
        break;
      }
      x = new_x;
      iter++;
      if (iter > MAX_ITER) {
        printf("ERREURE DE FATALE: Rebouclage de algorithme!\n");
        break;
      }
    }
    printf("Parametre:                     %1.20lf\n", alpha);
    printf("Precision:                     %g\n", prec);
    printf("Erreur:                        %g\n", g(x));
    printf("Simple iteration methode:      x = %1.20lf\n", x);
    printf("Preiterations:                 %d\n", pre_iter);
    printf("Iterations:                    %d\n", iter);
  }

  return 0;
}

int method_bisection(double prec) {
  int iter = 0;
  double x1 = alpha;
  double x2 = alpha + 1;
  printf("x1 = %lf\n", g(x1));
  printf("x2 = %lf\n", g(x2));

  if (g(x1) * g(x2) > 0) {
    printf("ERREURE DE FATALE: Irreguliere etat initial\n");
    getch();
    return -2;
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
    iter++;
    if (iter > 1000)
      break;
  }
  printf("Bisection methode:             x = %1.20lf\n", m);
  printf("Iterations:                    %d\n", iter);
  printf("Erreure:                       %g\n", g(m));
  return 0;
}

int method_newton(double prec) {
  int iter = 0;
  double x_0 = 2 * alpha;
  double x = x_0;
  double xp = x_0 + 0.1;
  while (true) {
    double xn = x - g(x) * (x - xp) / (g(x) - g(xp));
    xp = x;
    x = xn;
    if (fabs(g(x)) < prec)
      break;
    iter++;
    if (iter > 1000)
      break;
  }
  printf("De Newton methode:             x = %1.20lf\n", x);
  printf("Iterations:                    %d\n", iter);
  printf("Erreure:                       %g\n", g(x));
  return 0;
}

void hrule(int count) {
  for (int i = 0; i < count; i++)
    printf("=");
  printf("\n");
}

int main() {
  printf("Racine trouverer by ]DichlofoS[ Systems, Inc. Copyright (C) 2005\n");
  double main_prec = 1e-15;
  printf("Entrez de parametre ALPHA: ");
  scanf("%lf", &alpha);
  hrule(57);
  // for (double mu_zoom = .1; mu_zoom < 1; mu_zoom += .1)
  method_iteration(main_prec, .99);
  hrule(57);
  method_bisection(main_prec);
  hrule(57);
  method_newton(main_prec);
  hrule(57);
  getch();
  // graph();
  return 0;
}
