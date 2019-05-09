#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void user_input(int *Nx,int *Ny, int *Nz,float *x0, float  *xN,float *y0,float *yN,float *z0,float *zN,float *hx, float *hy,float *hz,int *Ntot)
{
        //MODULE TO GET THE USER INPUT
        printf("NUMBER OF POINTS IN X,Y & Z\n");
        scanf("%d %d %d",Nx,Ny,Nz);
        printf("INITIAL AN FINAL POINTS IN X COORDINATE\n");
        scanf("%f %f",x0,xN);
        printf("INITIAL AN FINAL POINTS IN Y COORDINATE\n");
        scanf("%f %f",y0,yN);
        printf("INITIAL AN FINAL POINTS IN Z COORDINATE\n");
        scanf("%f %f",z0,zN);



        *hx = (*xN-(*x0))/(float)(*Nx-1);
        *hy = (*yN-(*y0))/(float)(*Ny-1);
        *hz = (*zN-(*z0))/(float)(*Nz-1);

        *Ntot = (*Nx)*(*Ny)*(*Nz);
}
void evaluation(float *f,int Nx,int Ny,int Nz,int Ntot,float x0,float y0,float z0,float hx,float hy,float hz)
{
	float r,r2,x,y,z,val;
	int idx = 0;
	for(int i=0; i<Nx; i++)
	{
		x = x0 + i*hx;
		for(int j=0; j<Ny; j++)
		{
			y = y0 + j*hy;
			for(int k=0; k<Nz; k++)
			{
				z = z0 + k*hz;

				r2 = x*x + y*y + z*z;
				r = sqrt(r2);
				val = exp(-r);
				val = val*(3.0*z*z - r2);
				f[idx] = r2;
				printf("%10.6f %10.6f %10.6f %10.6f\n",f[idx],x,y,z);
				idx++;
			}
		}
	}
}
int main(void)
{
	int Nx,Ny,Nz,Ntot;
        float x0,xN,y0,yN,z0,zN,hx,hy,hz;
       

	user_input(&Nx,&Ny,&Nz,&x0,&xN,&y0,&yN,&z0,&zN,&hx,&hy,&hz,&Ntot);

	float f[Ntot];

	evaluation(f,Nx,Ny,Nz,Ntot,x0,y0,z0,hx,hy,hz);
	/*for(int i=0; i<Ntot; i++)
	{
            printf("work_item[%d] = %10.6f\n",i,f[i]);
	}*/


	return 0;


}
