#include "DMVN_lib.h"
#include "stdio.h"

int main()
{
	// polynom test
	try
	{
		double cE[1] = {1.0};
		TPolynom E(0, cE);
		double cB[2] = {-1.0, 1.0};
		TPolynom B(1, cB);

		for (int I = 0; I < 10; I++) (E *= B).Print(); 
		for (int I = 0; I < 10; I++) (E /= B).Print(); 
	}
	catch (TPolynom::Exception& e) { printf("Error: %s.\n", e.EMsg); }

	// substitution test
	const int max = 10;
	int s[max];
	for (int i =0; i < max;i++) s[i] = (i+1)%max;
	TSubst S(s, max);
	for (int i=0; i< 2; i++) { S *= S; S.Print(); }

	// matrix  test
	try
	{
		const int N = 2000;
		TMatrix A(N,N); // The Gilbert Matrix
		int k=0;
		for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++) { k++; A[j][i] = k; }// / double(i+j+1);
		// A.Print();
		printf("Determinant: %.20lf\n", A.Det());
	}
	catch (TMatrix::Exception& e) { printf("Error: %s.\n", e.EMsg); }


	return 0;

}
