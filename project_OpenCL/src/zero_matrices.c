#include<stdio.h>
#include <stdlib.h>

void FillZeroMat(int *mat,int M,int N)
{
	for(int i=0; i<M*N; i++)
	{
		mat[i] = 0;
	}

}
