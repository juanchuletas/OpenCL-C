#include <stdio.h>
#include <stdlib.h>

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
    
    printf("STEPS IN X = %f, STEPS IN Y = %f, STEPS IN Z = %f\n",*hx,*hy,*hz);
    printf("TOTAL POINTS = %d\n",*Ntot);
}

