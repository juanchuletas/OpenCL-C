#include <stdio.h>
#include <stdlib.h>
void ComputeAdjMat(int *admat,int n_reg,char filename[256])
{
	FILE *input;
	int Nreg;
	int num;
	if((input = fopen(filename,"r"))==NULL)
        {
                printf("FAILED TO OPEN FILE\n");
        }
	fscanf(input,"%d%*[^\n]\n",&Nreg);
        if(n_reg!=Nreg)
        {
                printf("WRONG FILE: NUMBRE OF REGIONS ARE DIFFERENT FROM THE GRAPH FILE\n");
        }

	int j;
	int k=0;
	int Gsize = Nreg*Nreg;
	fscanf(input,"%d",&num);
        while(!feof(input))
        {
		if(num!=-1)
		{
			j=num;
			admat[k*Nreg + j]=1;
		}
		else
		{
			k++;
		}
		fscanf(input,"%d",&num);
        }
	fclose(input);




}
