#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "matrices_functions.h"
#include "matrices_solving.h"
#include "get_time.h"

typedef struct _ARGS {
  double* A;
  double* b;
  double* x;
  double* c1;
  double* c2;
  double* c3;
  double* c4;
  double* c5;
  int n;
  int block_n;
  int thread_num;
  int total_threads;
} ARGS;

void* matrice_solve_Chkolecski_threaded (void *pa)
{
  ARGS* pargs = (ARGS*)pa;
  int res;
  long int t;
  
  pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  printf ("Thread %d started\n", pargs->thread_num);
  t = get_time ();
  res = Solve_Chkolecski (pargs->A, pargs->b, pargs->x, 
                          pargs->c1, pargs->c2, pargs->c3, pargs->c4, pargs->c5,
                          pargs->n,  pargs->block_n, 
			  pargs->thread_num, pargs->total_threads); 
  t = get_time () -t;
  printf ("Thread %d done, res=%d, time=%ld\n", pargs->thread_num, res, t);

  return (void*)res;
}

int main(int argc, char *argv[])
{
  int n, block_n, key, total_threads;
  pthread_t* threads;
  ARGS* args;
  int res = 0;
  int i;
  long int t;

  double *a, *b, *x, *c1, *c2, *c3, *c4, *c5, *d;
  FILE *fp;

  if (argc != 5) {
    printf ("Input matrix dimensioun(nxn):\n");
    if(scanf ("%d", &n) != 1 || n < 1) {
      printf ("Error: Invalid dimensioun!\n");
      return -1;
    }
    printf ("Input matrix block dimensioun:\n");
    if(scanf ("%d", &block_n) != 1 || block_n < 1 || block_n > n) {
      printf ("Error: Invalid block dimensioun!\n");
      return -1;
    }
    printf ("Input matrice initialization: 1 - from file, 2 - by function.\n");
    if(scanf ("%d", &key) != 1) {
      printf ("Error! Invalid key.\n");
      return -1;
    }  
    printf ("Input number of threads.\n");
    if(scanf ("%d", &total_threads) != 1 || 
                       total_threads < 1 || 
		       (n / block_n) / total_threads == 0) {
      printf ("Error! Too many threads.\n");
      return -1;
    }  
  } else {
    n = atoi (argv[1]);
    block_n = atoi (argv[2]);
    key = atoi (argv[3]);
    total_threads = atoi (argv[4]);
  }

  if (!(a = (double *)malloc (n * n * sizeof(double)))) {
    printf ("Error: Not enough memory for matrice A!\n");
    return -1;
  }

  if (!(b = (double *)malloc (n * sizeof(double)))) {
    printf ("Error: Not enough memory for vector b!\n");
    free (a);
    return -1;
  }

  if (!(x = (double *)malloc (n * sizeof(double)))) {
    printf ("Error: Not enough memory for vector x!\n");
    free (a); free (b);
    return -1;
  }

  if (!(c1 = (double *)malloc (block_n * block_n * total_threads * sizeof(double)))) {
    printf ("Error: Not enough memory for matrice c1!\n");
    free (a); free (b); free (x);
    return -1;
  }

  if (!(c2 = (double *)malloc (block_n * block_n * total_threads * sizeof(double)))) {
    printf ("Error: Not enough memory for matrice c2!\n");
    free (a); free (b); free (x); free (c1);
    return -1;
  }

  if (!(c3 = (double *)malloc ((block_n * n + (n % block_n) * (n % block_n)) * sizeof(double)))) {
    printf ("Error: Not enough memory for matrice c3!\n");
    free (a); free (b); free (x); free (c1); free (c2);
    return -1;
  }

  if (!(c4 = (double *)malloc (block_n * block_n * total_threads * sizeof(double)))) {
    printf ("Error: Not enough memory for matrice c4!\n");
    free (a); free (b); free (x); free (c1); free (c2); free (c3);
    return -1;
  }

  if (!(c5 = (double *)malloc (block_n * block_n * total_threads * sizeof(double)))) {
    printf ("Error: Not enough memory for matrice c5!\n");
    free (a); free (b); free (x); free (c1); free (c2); free (c3); free (c4);
    return -1;
  }

  if (!(threads = (pthread_t*) malloc (total_threads * sizeof (pthread_t)))){
    printf ("Not enough memory!\n");
    free (a); free (b); free (x); free (c1); free (c2); free (c3); free (c4); free (c5);
    return -1;    
  }

  if (!(args = (ARGS*) malloc (total_threads * sizeof (ARGS)))){
    printf ("Not enough memory!\n");
    free (a); free (b); free (x); free (c1); free (c2); free (c3); free (c4); free (c5);
    free (threads);
    return -1;    
  }

  switch (key) {
  case 1 : if(!(fp = fopen ("a.dat", "r"))) {
             printf ("Error: No file a.dat!\n");
             free (a); free (b); free (x); free (c1); free (c2); free (c3); free (c4); free (c5);
             free (threads); free (args);
             return -1;
           }
           if (Read_Matrix(a, b, fp, n) != n * (n + 1)) {
             printf ("Error reading file a.dat!\n");
             free (a); free (b); free (x); free (c1); free (c2); free (c3); free (c4); free (c5);
             free (threads); free (args);
             fclose (fp);
             return -1;
           }
           fclose(fp);
  	   break;
  case 2 : Init_A(a, n, block_n);
           Init_b (a, b, n, block_n);
	   break;
 default : printf("Error: Invalid key!\n");
           free (a); free (b); free (x); free (c1); free (c2); free (c3); free (c4); free (c5);
           free (threads); free (args);
           return -1;
  }

  Print_Matrix (a, "A was:", n);

  for (i = 0; i < total_threads; i++) {
    args[i].A = a;
    args[i].b = b;
    args[i].x = x;
    args[i].c1 = c1 + i * block_n * block_n;
    args[i].c2 = c2 + i * block_n * block_n;
    args[i].c3 = c3;
    args[i].c4 = c4 + i * block_n * block_n;
    args[i].c5 = c5 + i * block_n * block_n;
    args[i].n = n;
    args[i].block_n = block_n;
    args[i].thread_num = i;
    args[i].total_threads = total_threads;
  }

  t = get_full_time ();
  for (i = 0; i < total_threads; i++){
    if (pthread_create (threads + i, 0, matrice_solve_Chkolecski_threaded, args + i)) {
      printf ("cannot create thread #%d!\n", i);
      free (a); free (b); free (x); free (c1); free (c2); free (c3); free (c4); free (c5);
      free (threads); free (args);
      return -1;
    }
  }
  pthread_join (threads[0], (void*)(&res));
  if (res == 0) {
    for (i = 1; i < total_threads; i++) 
      pthread_cancel (threads[i]);
    for (i = 1; i < total_threads; i++) 
      pthread_join (threads[i], NULL);
    free (c1); free (c2); free (c3); free (c4); free (c5);
    free (threads); free (args);
    printf ("Unable to solve system!\n");
    return 1;
  } else 
    for (i = 1; i < total_threads; i++)
      if (pthread_join (threads[i], NULL)) {
        printf ("cannot join thread #%d!\n", i);
      }
  t = get_full_time () - t;

  Print_Vector (x, "Solution", n);

  free (c1); free (c2); free (c3); free (c4); free (c5); 
  free (threads); free (args);
  switch (key) {
  case 1 : if(!(fp = fopen ("a.dat", "r"))) {
             printf ("Error: No file a.dat!\n");
             free (a); free (b); free (x);
             return -1;
           }
           if (Read_Matrix(a, b, fp, n) != n * (n + 1)) {
             printf ("Error reading file a.dat!\n");
             free (a); free (b); free (x);
             fclose (fp);
             return -1;
           }
           fclose(fp);
  	   break;
  case 2 : Init_A(a, n, block_n);
           Init_b (a, b, n, block_n);
	   break;
  }

  if (!(d = (double *)malloc (n * sizeof(double)))) {
    printf ("Error: Not enough memory for vector d!\n");
    free (a); free (b); free (x);
    return -1;
  }

  if (key == 2) {
    for (i = 0; i < n; i += 2)
      d[i] = x[i] - 1;
    for (i = 1; i < n; i += 2)
      d[i] = 0;
    printf ("Norm of x-b is %e\n", Norm_Vector (d, n));
  }
  printf ("Residual %e\n", Residual (a, x, b, d, n));

  printf ("Time %lf\n", (double)t / 100);

  free (a); free (b); free (d); free (x);
  return 0;
}
