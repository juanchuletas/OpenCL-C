#include <stdio.h>
#include <stdlib.h>
void RandPopulation(int *pop_mat,int N_ind,int N_reg,int color)
{
	for(int i=0; i<N_ind; i++)
	{
		for(int j=0; j<N_reg; j++)
		{
			pop_mat[i*N_reg + j] = rand()%color;
		}
	}
}
