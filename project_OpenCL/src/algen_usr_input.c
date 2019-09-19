#include <stdio.h>
#include <stdlib.h>
void AlGenUsrInput(int *N_ind,int *N_color,int *M,int *N, int *N_reg, char filename[256])
{
	do
        {
		printf("ENTER THE POULATION SIZE\n");
                scanf("%d",N_ind);

        }while((*N_ind)%2!=0);
        printf("ENTER THE COLOUR REGIONS SIZE MxN\n");
        scanf("%d   %d",M,N);
        printf("ENTER THE No. OF COLOURS\n");
        scanf("%d",N_color);
        printf("GRAPH FILE NAME\n");
        scanf("%s",filename);
        *N_reg = (*M)*(*N);

}
