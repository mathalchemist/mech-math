#include <math.h>
#include <stdio.h>

#define C(i,j) c[(i-1)*(N-1)+j-1]
#define D(i,j) d[(i-1)*(N-1)+j-1]

#define F(i,j) f[(i)*(N-1)+j-1] 
#define Y(i,j) y[(i)*(N-1)+j-1]

#define Q(i,j) q[(i-1)*(N-1)+j-1]
#define P(i,j) p[(i-1)*(N-1)+j-1]
#define V(i,j) v[(i-1)*(N-1)+j-1]

double h = 0;
int N;
int n;

///////////////////////////////////////////////////////////
double ff(int i,int j)
{
    double x = double(i)*h;
	double y = double(j)*h;
	return 0;
	//return -2*x+2*y;
// return cos(2.0*x)*sin(2.0*y);
}

double g(int i,int j)
{
    double x = double(i)*h;
	double y = double(j)*h;
	return x*y;//.35*(x+y); 
//	return .35*(x+y); 
//	return x*x*x/3-y*y*y/3;
// sin(2.0*x)*cos(1.0*y);
}
///////////////////////////////////////////////////////////


void pm(double *c)
{
  //  return;
    for (int i = 1; i < N; i++)
    {
        for (int j = 1; j < N; j++)
        {
            printf("%6.1lf ", C(i,j));

        }
        printf("\n");
    }
    printf("=====\n");

}
void fill_c(double*c)
{
    for (int j = 1; j < N; j++)
        for (int i = 1; i < N; i++) C(i,j) = 0;
    for (int i = 1; i < N; i++) C(i,i) = 4;

    for (int i = 1; i < N-1; i++) C(i,i+1) = -1;
    for (int i = 2; i < N; i++) C(i,i-1) = -1;
}



void fill_cl(double *c, int k, int l)
{
    for (int j = 1; j < N; j++)
        for (int i = 1; i < N; i++) C(i,j) = 0;
    for (int i = 1; i < N; i++) C(i,i) = 4 - 2.0*cos(double(2*l-1)*3.1415926/double(1<<k));

    for (int i = 1; i < N-1; i++) C(i,i+1) = -1;
    for (int i = 2; i < N; i++) C(i,i-1) = -1;
}

int InvMatrix(int size, double *a, double *x);



int main()
{

	printf("Enter n: ");
	scanf("%d", &n);
	if (n < 2 || n >100) { printf ("wrong number.\n"); return 0; }
	
	N = (1<<n);
	h = 1.0/double(N);

/////////////////////////
	double *vv = new double[N-1];
/////////////////////////

    double *f;double *c;double *d;double *q;double *v;double *p;double *y;

//    printf("alloc...\n");
    int W=(N-1)*(N-1);
    int WE=(N+1)*(N-1);
    c = new double[W];d = new double[W];f = new double[WE];
    q = new double[W];p = new double[W];v = new double[W];
    y = new double[WE];


    fill_c(c);
	
    // step 1:
    // k = 0
    // q_j(0)  = F_j.

	
    for (int i = 1; i < N; i++) F(0,i) = g(0,i);
    for (int i = 1; i < N; i++) F(N,i) = g(N,i);

	for (int j = 1; j < N; j++)
	{
        for (int i = 1; i < N; i++)
        {
            F(j,i) = ff(i,j)*h*h;
        }
		F(j,1) += g(j,0);
		F(j,N-1) += g(j,N);
	}

    for (int j = 1; j < N; j++)
        for (int i = 1; i < N; i++)
        {
            Q(j,i) = F(j,i);
        }


     // step 2: k=1

    // fill p_j = 0
    for (int j = 1; j < N; j++)
        for (int i = 1; i < N; i++) P(j,i)=0.0;

    InvMatrix(N-1,c,d);
	

    for (int j = 2; j <= N-2; j+= 2)  // vectors
    {
        for (int i = 1; i < N; i++)   // coords
        {
            P(j,i) = 0;
            for (int s = 1; s < N; s++) P(j,i) += D(i,s)*Q(j,s); // sum
        }

    }    

    for (int j = 2; j <= N-2; j+= 2) // vects
    {
        for (int i = 1; i < N; i++)  // coords
            Q(j,i) = 2.0*P(j,i) + Q(j-1,i)+Q(j+1,i);
    }

    // step 3  k = 2,3,...N-1


    
    for (int k = 2; k < n;k++) 
    {
        // calc v_j^(0)
        int t = 1<<k;
        int m =  1<<(k-1);
        for (int j= t; j <= N-t; j+= t)
        {

            for (int i = 1; i < N; i++)
                V(j,i) = Q(j,i) + P(j-m,i)+P(j+m,i);

        }

        for (int l = 1; l <= m; l++) // step
        {
            fill_cl(c,k,l);
            InvMatrix(N-1,c,d);
            
            for (int j= t; j <= N-t; j+= t) // vector
            {
                for (int i = 1; i < N; i++) // coord
                {
                    double r = 0;
                    for (int s = 1; s < N; s++) r += D(i,s)*V(j,s); // sum
                    vv[i-1] = r;
                }
                for (int i = 1; i < N; i++) V(j,i) = vv[i-1];				
            }           
        }
        // next...
        for (int j= t; j <= N-t; j+= t) // vector
        {
            for (int i = 1; i < N; i++)
                P(j,i) = P(j,i) + V(j, i);
            for (int i = 1; i < N; i++)
                Q(j,i) = 2.0*P(j,i) + Q(j-m, i)+Q(j+m,i);
        } 

    }

    // reverse step ============

    for (int i = 1; i < N; i++)
    {
        Y(0,i) = F(0,i);
        Y(N,i) = F(N,i);
    }



    for (int k = n; k >= 2; k--)
    {
        int m = 1 << (k-1);
        int t = 1 << k;
        for (int j = m; j <= N-m; j += 2*m)
        {
            for (int i = 1; i < N; i++)
                V(j,i) = Q(j,i) + Y(j-m,i) + Y(j+m,i);
        }

        for (int l = 1; l <= m; l++) // l = 1..2^{k-1}
        {
            fill_cl(c,k,l);
            InvMatrix(N-1,c,d);
            for (int j= m; j <= N-m; j+= 2*m) // vector
            {
                for (int i = 1; i < N; i++) // coord
                {
                    double r = 0;
                    for (int s = 1; s < N; s++) r += D(i,s)*V(j,s); // sum
                    vv[i-1] = r;
                }
                for (int i = 1; i < N; i++) V(j,i) = vv[i-1];				
            }           
        }
        for (int j= m; j <= N-m; j+= 2*m) // vector
        {
            for (int i = 1; i < N; i++) Y(j,i) = P(j,i) + V(j,i);
        }

    }
 // k =1
    fill_c(c);
    InvMatrix(N-1,c,d);

    for (int j = 1; j <= N-1; j +=2)
    {
        for (int i = 1; i < N; i++)
        {
            double r = 0;
            for (int s = 1; s < N; s++)
                r += (D(i,s))*(Q(j,s)+Y(j-1,s)+Y(j+1,s));
            vv[i-1] = r;
        }
		for (int i = 1; i < N; i++) Y(j,i) = vv[i-1];
    }

    // print...
	FILE*FFF = fopen("a.mp", "w+");
	

    for (int j = 1; j < N-1; j++)
    {
		for (int i = 1; i < N-1; i++)
        {
            fprintf(FFF,"draw dpr((%1.3lf*u, %1.3lf*u, %1.3lf*u))--dpr((%1.3lf*u, %1.3lf*u, %1.3lf*u));\n", double(i)*h, double(j)*h, Y(j,i),  double(i+1)*h, double(j)*h, Y(j,i+1));
            fprintf(FFF,"draw dpr((%1.3lf*u, %1.3lf*u, %1.3lf*u))--dpr((%1.3lf*u, %1.3lf*u, %1.3lf*u));\n", double(i)*h, double(j)*h, Y(j,i),  double(i)*h, double(j+1)*h, Y(j+1,i));

        }
        fprintf(FFF,"\n");
    }
	
	fprintf(FFF,"pickup pencircle scaled .25pt\n");
    for (int j = 1; j < N-1; j++)
    {
		for (int i = 1; i < N-1; i++)
        {
            fprintf(FFF,"draw dpr((%1.3lf*u, %1.3lf*u, %1.3lf*u))--dpr((%1.3lf*u, %1.3lf*u, %1.3lf*u)) wc red;\n", double(i)*h, double(j)*h, g(j,i),  double(i+1)*h, double(j)*h, g(j,i+1));
            fprintf(FFF,"draw dpr((%1.3lf*u, %1.3lf*u, %1.3lf*u))--dpr((%1.3lf*u, %1.3lf*u, %1.3lf*u)) wc red;\n", double(i)*h, double(j)*h, g(j,i),  double(i)*h, double(j+1)*h, g(j+1,i));

        }
        fprintf(FFF,"\n");
    }

	double err = 0.0;
    for (int j = 1; j < N-1; j++)
    {
		for (int i = 1; i < N-1; i++)
        {
            double delta = fabs(g(j,i)-Y(j,i));
			if (delta > err) err = delta;

        }
    }
	fprintf(FFF,"\nlabel(btex Error: $%1.15lf$. etex, (0,-.6u));\n", err);
	fclose(FFF);

    delete c; delete d; delete f; delete q; delete p; delete v; delete y;
	delete vv;

    return 0;
}


#define mItem(a, i, j) a[(i) * size + j]

int InvMatrix(int size, double *a, double *x)
{
	int i, j, k;
	double p;

	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			mItem(x, i, j) = (i == j);

	/* Gauss forward move */
	for (i = 0; i < size; i++)
	{
		if (fabs(p = mItem(a, i, i)) < 1e-100)
			return 0;

		for (k = 0; k < size; k++)
		{
			mItem(a, i, k) /= p;
			mItem(x, i, k) /= p;
		}

		for (j = i + 1; j<size; j++)
		{
			p = mItem(a, j, i);
			for (k = 0; k < size; k++)
			{
				mItem(a, j, k) -= p * mItem(a, i, k);
				mItem(x, j, k) -= p * mItem(x, i, k);
			}
		}
	}

	/* Gauss back move */
	for (k = 0; k < size; k++)
		for (i = size - 1; i >= 0; i--)
		{
			p = mItem(x, i, k);
			for (j = i + 1; j < size; j++)
				p -= mItem(x, j, k) * mItem(a, i, j);
			mItem(x, i, k) = p/mItem(a, i, i);
		}

	return 1;
}

