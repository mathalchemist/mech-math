#include<iostream.h>
#include<stdio.h>

int main(){
	

	int M[300], A[300];
	M[0]=1;
	for(int i=1; i<300; i++){M[i]=0;A[i]=0;}

	int k, j;
	for(i=1; i<=100; i++){	
	
		for( k=0; k<300; k++){
			A[k]=M[k]*i;            
		}
		
		for( j=0; j<300; j++){
				M[j]=A[j]%10;
				A[j+1]+=A[j]/10;
			}
	
					
	}

cout<<"100!=";

	int L=299;
	while(M[L]==0)
	{
	   L--;	
	}
for(i=L; i>=0; i--)cout<<M[i]<<" ";
cout<<"\n";



return 0;
}