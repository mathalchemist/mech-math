#include "matrices_solving.h"
#include "synchronize.h"
#define ZERO 1e-16

//заполняет массиы матрик block_n x block_n + rest_n x rest_n единичными матрицами 
static inline void INIT_E (double *A, int block_n, int block_count, int rest_n)
{
  int i, j;
  
  for (i = 0; i < block_count; i++) {
    for (j = 0; j < block_n - 1; j++) {
      *A = 1; A++;
      memset (A, 0, block_n * sizeof (double));
      A += block_n;
    }
    *A = 1; A++;    
  }    

  if (rest_n != 0) {
    for (i = 0; i < rest_n - 1; i++) {
      *A = 1; A++;
      memset (A, 0, rest_n * sizeof (double));
      A += rest_n;
    }
    *A = 1;
  }    
  
}



//вычисляет A_t x B x C, передаются матрица A нетранспонипованная
static inline void MUL1 (double *A, double *B, double *C, double *T1, double *T2, int block_n)
{
  int i, j, k;
  double f;

  double *t1 = T1, *c = C, *a = A; 
//B x C результат в T1
  for (i = 0; i < block_n; i++, B += block_n) 
    for (j = 0; j < block_n; j++, T1++) {
      C = c + j;
      f = 0;
      for (k = 0; k < block_n; k++, C += block_n)
        f += B[k] * (*C);
      *T1 = f;
    }
  T1 = t1;
//A_t x T1, результат в T2
  for (i = 0; i < block_n; i++) 
    for (j = 0; j < block_n; j++, T2++) {
      T1 = t1 + j;
      A = a + i;
      f = 0;
      for (k = 0; k < block_n; k++, A += block_n, T1 += block_n)
        f += (*A) * (*T1);
      *T2 = f;
    }
}

static inline void SUB1 (double *M, double *N, int n, int block_n)
{
  int i, j;
  
  for (i = 0; i < block_n; i++, M += n)
    for (j = 0; j < block_n; j++, N++)
      M[j] -= *N;
}

//вычисляет A_t x B x C, передаются матрица A нетранспонипованная
static inline void MUL2 (double *A, double *B, double *C, double *T1, double *T2, int block_n, int rest_n)
{
  int i, j, k;
  double f;
  double *t1 = T1, *c = C, *a = A; 
//B x C результат в T1
  for (i = 0; i < block_n; i++, B += block_n) 
    for (j = 0; j < rest_n; j++, T1++) {
      C = c + j;
      f = 0;
      for (k = 0; k < block_n; k++, C += rest_n)
        f += B[k] * (*C);
      *T1 = f;
    }
  T1 = t1;
//A_t x T1, результат в T2
  for (i = 0; i < block_n; i++) 
    for (j = 0; j < rest_n; j++, T2++) {
      T1 = t1 + j;
      A = a + i;
      f = 0;
      for (k = 0; k < block_n; k++, A += block_n, T1 += rest_n)
        f += (*A) * (*T1);
      *T2 = f;
    }
}

static inline void SUB2 (double *M, double *N, int n, int block_n, int rest_n)
{
  int i, j;
  
  for (i = 0; i < block_n; i++, M += n)
    for (j = 0; j < rest_n; j++, N++)
      M[j] -= *N;
}

//вычисляет A_t x B x C, передаются матрица A нетранспонипованная
static inline void MUL3 (double *A, double *B, double *C, double *T1, double *T2, int block_n, int rest_n)
{
  int i, j, k;
  double f;
  double *t1 = T1, *c = C, *a = A; 
//B x C результат в T1
  for (i = 0; i < block_n; i++, B += block_n) 
    for (j = 0; j < rest_n; j++, T1++) {
      C = c + j;
      f = 0;
      for (k = 0; k < block_n; k++, C += rest_n)
        f += B[k] * (*C);
      *T1 = f;
    }
  T1 = t1;
//A_t x T1, результат в T2
  for (i = 0; i < block_n; i++) 
    for (j = 0; j < rest_n; j++, T2++) {
      T1 = t1 + j;
      A = a + i;
      f = 0;
      for (k = 0; k < block_n; k++, A += rest_n, T1 += rest_n)
        f += (*A) * (*T1);
      *T2 = f;
    }
}

static inline void SUB3 (double *M, double *N, int n, int rest_n)
{
  int i, j;
  
  for (i = 0; i < rest_n; i++, M += n)
    for (j = 0; j < rest_n; j++, N++)
      M[j] -= *N;
}

static inline int Back_Jordan_C(double *A, double *B, int n)
{
  int i, j, k, l, r;
  double tmp, max;

  for (i = 0; i < n; i++) {
    max = 0;
    r = l = i;
    for (j = i; j < n; j++, l += n)
      if (fabs(A[l]) > max) {
        max = fabs(A[l]);
        r = j;
      }
    if (r != i)	{
      l = 0;
      r = (r - i) * n;
      for (k = 0; k < i; k++, l++, r++) {
        tmp  = B[l];
        B[l] = B[r];
        B[r] = tmp;
      }
      for (k = i; k < n; k++, l++, r++) {
        tmp  = B[l];
        B[l] = B[r];
        B[r] = tmp;
        tmp  = A[l];
        A[l] = A[r];
        A[r] = tmp;
      }
    }
    if (fabs(max) > 0) {
      tmp = 1 / A[i];
      for (j = 0; j <= i; j++)
        B[j] *= tmp;
      for (j = i + 1; j < n; j++) {
        A[j] *= tmp;
        B[j] *= tmp;
      }
    } else
      return 0;
    l = -i *n;
    for (j = 0; j < i; j++) {
      tmp = -A[l+i];
      if (fabs(tmp) <= ZERO) {
        l += n;
        continue;
      }
      for (k = 0; k <= i; k++, l++)
        B[l] += B[k] * tmp;
      for (k = i + 1; k < n; k++, l++) {
        B[l] += B[k] * tmp;
        A[l] += A[k] * tmp;
      }
    }
    l = n;
    for (j = i + 1; j < n; j++)	{
      tmp = -A[l+i];
      if (fabs(tmp) <= ZERO) {
        l += n;
        continue;
      }
      for (k = 0; k <= i; k++, l++)
        B[l] += B[k] * tmp;
      for (k = i + 1; k < n; k++, l++) {
        B[l] += B[k] * tmp;
        A[l] += A[k] * tmp;
      }
    }
    A += n;
    B += n;
  }

  return 1;
}

//представление матрицы A:
//|A_11 A_12 .. A_1n    a1n+1|
//|A_21 A_22 .. A_2n    a2n+1|
//|:         ..         ..  :|
//|A_n1 A_n2 .. A_nn    ann+1|
//|an+11     .. ann+1 an+1n+1|
//A_ij - квадратные block_n x block_n, а a_ij - прямоугольные и квадратные соотв
//block_n * .. и .. x ..

int 
Solve_Chkolecski
(double *A,             //матрица системы, n x n
double *b,              //правая часть системы, n
double *x,              //решение, n
double *C1, double *C2, //временные матрицы для промежуточных операций, block_n x block_n
double *C3,             //массив матриц, обратных к диагональным матрицам, n x block_n + (n % block_n)^2 
double *C4, double *C5, //временные матрицы для перемножения, block_n x block_n
int n,                  //размер матрицы A
int block_n,            //размер блока
int thread_num,         //номер задачи
int total_threads       //  общее число задач
) 
{
  int block_count = n / block_n, rest_n = n % block_n; 
  int N = block_count, block_per_thread, first_block, last_block, additional_block = 0;
  int active_threads = total_threads;
  int i, j, k, m, l;
  int p, q;
  int block_n_x_block_n = block_n * block_n, n_x_block_n = n * block_n;
  
  double *B_ii, *B_ij, *B_kj, *B_ki, *B_ik, *B_kk;
  double *t1, *t2;
  double *sum, *sum_i, *x_k;
  double f;

//инициализируем сразу весь массив матрий для обращения единичными
 if (thread_num == 0)
    INIT_E (C3, block_n, block_count, rest_n);

//блочно строим матрицу B--------------------------------------------
  for (i = 0; i < block_count; i++, N--) {
    //Определяем число блоков на каждую задачу, пытась балансировать нагрузку:
    //если осталось active_threads-1 блоков, то распределяем их поровну между 1..active_threads-1,
    //при этом если есть неквадратный блок, то он обрабатывается задачей 0.
    //иначе оставшиеся блоки обрабатываются задачами 1..N%active_threads, а оставшийся неквадаратный
    //блок, если он есть, задачей N%active_threads+1.
    block_per_thread = N / active_threads;
    if (block_per_thread == 0) {
      active_threads--;
      block_per_thread = N / active_threads;
    }
    //работают только активные процессы
    if (thread_num <  active_threads) 
    {
      first_block = i + thread_num * block_per_thread;
      last_block = first_block + block_per_thread;

      if (N % active_threads == active_threads - 1) {
        if (thread_num != 0) 
	  additional_block = i + block_per_thread * active_threads + (thread_num - 1);
	else 
	  additional_block = (rest_n > 0 ? block_count : -1);
      } else {
        if (thread_num != 0) {
          if (thread_num <= N % active_threads)
            additional_block = i + block_per_thread * active_threads + (thread_num - 1);
          else { 
	    if (thread_num - (N % active_threads) == 1)
	      additional_block = (rest_n > 0 ? block_count : -1);
	    else
	      additional_block = -1;  
	  }
	} else
	  additional_block = -1;
      }
      if (thread_num == 0) {    
        //строим диагональный блок в нулевой задаче
        B_ii = A + (i * n + i) * block_n;
	for (k = thread_num; k < i; k++) {
          B_ki = A + k * n_x_block_n + i * block_n;
          B_kk = C3 + block_n_x_block_n * k;
          for (m = 0; m < block_n; m++)  
            memcpy (C4 + m * block_n, B_ki + m * n, block_n * sizeof (double));
          MUL1 (C4, B_kk, C4, C1, C2, block_n);
          SUB1 (B_ii, C2, n, block_n); 	
        }
        l = thread_num < i ? thread_num : i;
	for (k = 0; k < l; k++) {
          B_ki = A + k * n_x_block_n + i * block_n;
          B_kk = C3 + block_n_x_block_n * k;
          for (m = 0; m < block_n; m++)  
            memcpy (C4 + m * block_n, B_ki + m * n, block_n * sizeof (double));
          MUL1 (C4, B_kk, C4, C1, C2, block_n);
          SUB1 (B_ii, C2, n, block_n); 	
        }
        first_block++;
      }
      
      //строим остальные недиагональные блоки
      for (j = first_block; j < last_block; j++) {
        B_ij =  A + i * n_x_block_n + j * block_n;
        for (k = thread_num; k < i; k++) {
          B_ki = A + k * n_x_block_n + i * block_n;
          B_kk = C3 + block_n_x_block_n * k;
          B_kj = A + k * n_x_block_n + j * block_n;
          for (m = 0; m < block_n; m++) 
            memcpy (C4 + m * block_n, B_ki + m * n, block_n * sizeof (double));
          for (m = 0; m < block_n; m++) 
            memcpy (C5 + m * block_n, B_kj + m * n, block_n * sizeof (double));
          MUL1 (C4, B_kk, C5, C1, C2, block_n); 	
          SUB1 (B_ij, C2, n, block_n);
        }
        l = thread_num < i ? thread_num : i;
        for (k = 0; k < l; k++) {
          B_ki = A + k * n_x_block_n + i * block_n;
          B_kk = C3 + block_n_x_block_n * k;
          B_kj = A + k * n_x_block_n + j * block_n;
          for (m = 0; m < block_n; m++) 
            memcpy (C4 + m * block_n, B_ki + m * n, block_n * sizeof (double));
          for (m = 0; m < block_n; m++) 
            memcpy (C5 + m * block_n, B_kj + m * n, block_n * sizeof (double));
          MUL1 (C4, B_kk, C5, C1, C2, block_n); 	
          SUB1 (B_ij, C2, n, block_n);
        }
      }
    
      if (additional_block != -1) {
        j = additional_block;
        B_ij =  A + i * n_x_block_n + j * block_n;
        for (k = thread_num; k < i; k++) {
          B_ki = A + k * n_x_block_n + i * block_n;
          B_kk = C3 + block_n_x_block_n * k;
          B_kj = A + k * n_x_block_n + j * block_n;
          for (m = 0; m < block_n; m++) 
            memcpy (C4 + m * block_n, B_ki + m * n, block_n * sizeof (double));
          if (additional_block == block_count) {
            for (m = 0; m < block_n; m++) 
              memcpy (C5 + m * rest_n, B_kj + m * n, rest_n * sizeof (double));
            MUL2 (C4, B_kk, C5, C1, C2, block_n, rest_n); 	
            SUB2 (B_ij, C2, n, block_n, rest_n);
          } else {
            for (m = 0; m < block_n; m++) 
              memcpy (C5 + m * block_n, B_kj + m * n, block_n * sizeof (double));
            MUL1 (C4, B_kk, C5, C1, C2, block_n); 	
            SUB1 (B_ij, C2, n, block_n);
          }
        }
        l = thread_num < i ? thread_num : i;
        for (k = 0; k < l; k++) {
          B_ki = A + k * n_x_block_n + i * block_n;
          B_kk = C3 + block_n_x_block_n * k;
          B_kj = A + k * n_x_block_n + j * block_n;
          for (m = 0; m < block_n; m++) 
            memcpy (C4 + m * block_n, B_ki + m * n, block_n * sizeof (double));
          if (additional_block == block_count) {
            for (m = 0; m < block_n; m++) 
              memcpy (C5 + m * rest_n, B_kj + m * n, rest_n * sizeof (double));
            MUL2 (C4, B_kk, C5, C1, C2, block_n, rest_n); 	
            SUB2 (B_ij, C2, n, block_n, rest_n);
          } else {
            for (m = 0; m < block_n; m++) 
              memcpy (C5 + m * block_n, B_kj + m * n, block_n * sizeof (double));
            MUL1 (C4, B_kk, C5, C1, C2, block_n); 	
            SUB1 (B_ij, C2, n, block_n);
          }
        }
      }      
    }
    if (thread_num == 0 ) {
      B_ii = A + (i * n + i) * block_n;
      t1 = C1; t2 = B_ii;
      for (j = 0; j < block_n; j++, t2 += n, t1 += block_n)
        memcpy (t1, t2, block_n * sizeof (double));
      if (!Back_Jordan_C(C1, C3 + block_n_x_block_n * i, block_n)) return 0;
    }
    synchronize (total_threads);
  }

  if (thread_num != 0)
    return 1;
  //достраиваем оставшийся маленький квадрат, если он есть
  if (rest_n != 0) {
    B_ii =  A + block_count * block_n * n + block_count * block_n;
    for (k = 0; k < i; k++) {
      B_ki = A + k * block_n * n + block_count * block_n;
      B_kk = C3 + block_n_x_block_n * k;
      for (m = 0; m < block_n; m++)  
        memcpy (C4 + m * rest_n, B_ki + m * n, rest_n * sizeof (double));
      MUL3 (C4, B_kk, C4, C1, C2, block_n, rest_n);
      SUB3 (B_ii, C2, n, rest_n); 	
    }
    t1  = C1; t2 = B_ii;
    for (j = 0; j < rest_n; j++, t2 += n, t1 += rest_n)
      memcpy (t1, t2, rest_n * sizeof (double));
    if (!Back_Jordan_C(C1, C3 + block_n_x_block_n * block_count, rest_n)) return 0;
  }
  
//Обратный ход метода Гаусса-----------------------------------------
//1.Решаем B_tx=b
  sum = b;
  for (i = 0; i < block_count; i++) {
    B_ii = A + (i * n + i) * block_n;
    sum_i = sum + i * block_n;
    for (k = 0; k < i; k++) {
      B_ki = A + (k * n + i) * block_n;
      x_k = x + k * block_n;
      //дальше идет перемножение B_ki_t x X_k
      for (p = 0; p < block_n; p++) {
        for (f = 0, q = 0; q < block_n; q++)
          f += B_ki[q*n+p] * x_k[q];
        C1[p] = f;
      }
      //вычитаем C1 из sum_i
      for (p = 0; p < block_n; p++)
        sum_i[p] -= C1[p];
    }
    //дальше идет перемножение B_ii^-1_t X_k, но B_ii^-1_t = B_ii^-1
    t1 = C3 + i * block_n_x_block_n;
    t2 = x + i * block_n;
    for (p = 0; p < block_n; p++) {
      for (f = 0, q = 0; q < block_n; q++)
        f += t1[p*block_n+q] * sum_i[q];
      t2[p] = f;
    }      
  }
  if (rest_n != 0) {
    B_ii = A + block_count * block_n * n + block_count * block_n;
    sum_i = sum + block_count * block_n;
    for (k = 0; k < block_count; k++) {
      B_ki = A + (k * n + block_count) * block_n;
      x_k = x + k * block_n;
      //дальше идет перемножение B_ki_t x X_k
      for (p = 0; p < rest_n; p++) {
        for (f = 0, q = 0; q < block_n; q++)
          f += B_ki[q*n+p] * x_k[q];
        C1[p] = f;
      }
      //вычитаем C1 из sum_i
      for (p = 0; p < rest_n; p++)
        sum_i[p] -= C1[p];
    }
    //дальше идет перемножение B_ii^-1_t x X_i, но B_ii^-1_t = B_ii^-1
    t1 = C3 + block_count * block_n_x_block_n;
    t2 = x + block_count * block_n;
    sum_i = sum + block_count * block_n;
    for (p = 0; p < rest_n; p++) {
      for (f = 0, q = 0; q < rest_n; q++)
        f += t1[p*rest_n+q] * sum_i[q];
      t2[p] = f;
    }
  } 
//2.Решаем Hz=x
  for (i = 0; i < block_count; i++) { 
    B_ii = A + (i * n + i) * block_n;
    for (j = 0; j < block_n; j++) {
      for (f = 0, k = 0; k < block_n; k++)
        f += B_ii[j*n+k] * x[i*block_n+k];
      b[i*block_n+j] = f;
    }         
  }
  if (rest_n != 0) {
    //даьше идет перемножение B_ii x X_i
    B_ii = A + block_count * block_n * n + block_count * block_n;
    for (p = 0; p < rest_n; p++) {
      for (f = 0, q = 0; q < rest_n; q++)
        f += B_ii[p*n+q] * x[block_count*block_n+q];
      b[block_count*block_n+p] = f;
    }         
  }    

//3.Решаем B_y=z
  sum = b;
  if (rest_n != 0) {
    B_ii = A + block_count * block_n * n + block_count * block_n;
    sum_i = sum + block_count * block_n;
    //дальше идет перемножение B_ii^-1 x X_i
    t1 = C3 + block_count* block_n_x_block_n;
    t2 = x + block_count * block_n;
    for (p = 0; p < rest_n; p++) {
      for (f = 0, q = 0; q < rest_n; q++)
        f += t1[p*rest_n+q] * sum_i[q];
      t2[p] = f;
    }
  } 
  for (i = block_count - 1; i >= 0; i--) {
    B_ii = A + (i * n + i) * block_n;
    sum_i = sum + i * block_n;
    for (k = i + 1; k < block_count; k++) {
      B_ik = A + (i * n + k) * block_n;
      x_k = x + k * block_n;
      //дальше идет перемножение B_ik x X_k
      for (p = 0; p < block_n; p++) {
        for (f = 0, q = 0; q < block_n; q++)
          f += B_ik[p*n+q] * x_k[q];
        C1[p] = f;
      }
      //вычитаем C1 из sum_i
      for (p = 0; p < block_n; p++)
        sum_i[p] -= C1[p];
    }
    if (rest_n != 0) {
      //дальше идет перемножение B_ik x X_k
      B_ik = A + i * block_n * n + block_n * block_count;
      x_k = x + block_count * block_n;
      for (p = 0; p < block_n; p++) {
        for (f = 0, q = 0; q < rest_n; q++)
          f += B_ik[p*n+q] * x_k[q];
        C1[p] = f;
      }
      //вычитаем C1 из sum_i
      for (p = 0; p < block_n; p++)
        sum_i[p] -= C1[p];
    }
    //дальше идет перемножение B_ii^-1 X_k
    t1 = C3 + i * block_n_x_block_n;
    t2 = x + i * block_n;
    for (p = 0; p < block_n; p++) {
    for (f = 0, q = 0; q < block_n; q++)
      f += t1[p*block_n+q] * sum_i[q];
      t2[p] = f;
    }
  }

  return 1;        
}


