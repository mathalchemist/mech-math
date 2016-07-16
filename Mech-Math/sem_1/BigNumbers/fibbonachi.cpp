#include<iostream.h>
#include<stdio.h>

int main(){
	

	int A[300], B[300], C[300];
	A[0]=1;
	B[0]=1;
	for(int i=1; i<300; i++){A[i]=0;B[i]=0;C[i]=0;}

	int k, j;
	for(i=2; i<=1000; i++){	
	
		for( k=0; k<300; k++){
			C[k]=A[k]+B[k];            
			A[k]=B[k];
			B[k]=C[k];
		}
		
		for( j=0; j<300; j++){
				B[j]=C[j]%10;
				C[j+1]+=C[j]/10;
			}
	
					
	}

cout<<"Result=";

	int L=299;
	while(A[L]==0)
	{
	   L--;	
	}
for(i=L; i>=0; i--)cout<<A[i];
cout<<"\n";
 



return 0;
}