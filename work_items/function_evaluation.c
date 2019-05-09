//
//  id_checking.c
//  
//
//  Created by Juan García on 1/24/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define PROGRAM_FILE "fz_function.cl"
#define KERNEL_NAME_FUNC "fz_function"
#define Gx 256
#define Gy 256
#define Lx 16
#define Ly 16
#define N_WORK_ITEMS 64;


#ifdef __APPLE__
#define CL_SILENCE_DEPRECATION
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

void check_error(cl_int err, char *str)
{
    if(err<0)
    {
        printf("SORRY,THERE IS AN ERROR IN %s\n",str);
        exit(1);
    }
}



// FUNCTION TO SET THE DEVICES
cl_device_id CreateDevice()
{
    cl_platform_id platform;
    cl_device_id dev;
    cl_int err;
    
    err = clGetPlatformIDs(1,&platform,NULL);
    check_error(err, "DEVICE CREATION");
    err = clGetDeviceIDs(platform,CL_DEVICE_TYPE_GPU,1,&dev,NULL);
    if(err==CL_DEVICE_NOT_FOUND)
    {
        err = clGetDeviceIDs(platform,CL_DEVICE_TYPE_CPU,1,&dev,NULL);
    }
    check_error(err,"DEVICE CREATION");
    
    return dev;
}
cl_program BuildProgram(cl_context cntx, cl_device_id dev, const char *filename)
{
    cl_program program;
    cl_int err;
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size,log_size;
    
    program_handle = fopen(filename,"r");
    if(program_handle==NULL)
    {
        perror("FILE NOT FOUND");
        exit(1);
    }
    
    fseek(program_handle,0,SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char *)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer,sizeof(char),program_size,program_handle);
    fclose(program_handle);
    
    // CREATE THE PROGRAM FROM A FILE
    program = clCreateProgramWithSource(cntx,1,(const char**)&program_buffer,&program_size,&err);
    check_error(err,"PROGRAM CREATION");
    
    free(program_buffer);
    
    //BUILD THE PROGRAM
    err = clBuildProgram(program,0,NULL,NULL,NULL,NULL);
    if(err<0)
    {
        clGetProgramBuildInfo(program,dev,CL_PROGRAM_BUILD_LOG,0,NULL,&log_size);
        program_log = (char *)malloc(log_size+1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program,dev,CL_PROGRAM_BUILD_LOG,log_size+1,program_log,NULL);
        printf("%s\n",program_log);
        free(program_log);
        exit(1);
    }
    
    return program;
}
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
void get_3Dcube_file(int Nx,int Ny,int Nz,float x0,float y0,float z0,float hx,float hy,float hz,float *f,int Ntot)
{ 
	int i,j;
	FILE *cube_out; 

	cube_out=fopen("out.cube","w");
	if(cube_out==NULL)
	{
		perror("COULD NOT CREATE FILE\n");
		exit(1);
	}
	fprintf(cube_out,"CUBE FILE FORMAT 3D\n");
	fprintf(cube_out,"GRID EVALUATION\n");
	fprintf(cube_out,"%4d \t% 10.6f \t% 10.6f \t% 10.6f\n",1,x0,y0,z0);
	fprintf(cube_out,"%4d \t% 10.6f \t% 10.6f \t% 10.6f\n",Nx,hx,0.,0.);
	fprintf(cube_out,"%4d \t% 10.6f \t% 10.6f \t% 10.6f\n",Ny,0.,hy,0.);
	fprintf(cube_out,"%4d \t% 10.6f \t% 10.6f \t% 10.6f\n",Nz,0.,0.,hz);
	fprintf(cube_out,"%4d \t% 10.6f \t% 10.6f \t% 10.6f \t% 10.6f\n",3,3.,0.,0.,0.);
	for(i=0,j=0; i<Ntot; i++)
	{
		j++;
		fprintf(cube_out,"\t%10.6E",f[i]);
		if(j==6 || i==Ntot-1)
		{
			fprintf(cube_out,"\n");
			j=0;
		}
	}
	fclose(cube_out);
}
int main(void)
{
    cl_device_id device;
    cl_context context;
    cl_command_queue cQ;
    cl_program program;
    cl_kernel kernel;
    cl_int err;

    int Nx,Ny,Nz,Ntot;
    float x0,xN,y0,yN,z0,zN,hx,hy,hz;

    user_input(&Nx,&Ny,&Nz,&x0,&xN,&y0,&yN,&z0,&zN,&hx,&hy,&hz,&Ntot);

    printf("STEPS IN X = %f, STEPS IN Y = %f, STEPS IN Z = %f\n",hx,hy,hz);
    printf("TOTAL POINTS = %d\n",Ntot);

    float step[]={hx,hy,hz};
    float point0[]={x0,y0,z0};
    float test[Ntot];

    //WORK-ITEMS DATA******************
    size_t dim = 2;
    size_t global_offset[] = {0,0};
    size_t global_size[] = {Gx,Gy};
    size_t local_size[] = {Lx,Ly};

    //MEMORY OBJECTS********************
    cl_mem read_buff,buff_step,buff_points;
    // CREATE A DEVICE AND A CONTEXT
    device = CreateDevice(); // FUNCTION ABOVE
    context = clCreateContext(NULL,1,&device,NULL,NULL,&err);
    check_error(err,"CONTEXT CREATION");
    
    // BUILD THE PROGRAM AND CREATE THE KERNEL
    program = BuildProgram(context,device,PROGRAM_FILE);
    
    kernel = clCreateKernel(program,KERNEL_NAME_FUNC,&err);
    check_error(err,"KERNEL CREATION");
    
    //CREATE A WRITE ONLY BUFFER TO HOLD THE OUTPUT DATA
    read_buff = clCreateBuffer(context,CL_MEM_WRITE_ONLY,sizeof(test),NULL,&err);
    check_error(err,"BUFFER CREATION");
    buff_step = clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(step),step,&err);
    check_error(err,"BUFFER CREATION");
    buff_points = clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(point0),point0,&err);
    check_error(err,"BUFFER CREATION");
    
    //CREATE KERNEL ARGUMENT
    //check_error(err, "KERNEL SETTING ARGUMENT");
    err = clSetKernelArg(kernel,0,sizeof(cl_mem),&read_buff);
    err |= clSetKernelArg(kernel,1,sizeof(cl_mem),&buff_step);
    err |= clSetKernelArg(kernel,2,sizeof(cl_mem),&buff_points);
    err |= clSetKernelArg(kernel,3,sizeof(Ny),&Ny);
    err |= clSetKernelArg(kernel,4,sizeof(Nz),&Nz);
    err |= clSetKernelArg(kernel,5,sizeof(Ntot),&Ntot);
    check_error(err, "KERNEL SETTING ARGUMENT");
    
    // CREATING THE COMMAND QUEUE TO SEND INSTRUCTIONS TO THE DEVICE
    cQ = clCreateCommandQueue(context,device,0,&err);
    check_error(err, "COMMAND QUEUE CREATION");
    
    // ENQUEUE THE KERNEL
    err = clEnqueueNDRangeKernel(cQ,kernel,dim,global_offset,global_size,local_size,0,NULL,NULL);
    check_error(err,"THE KERNEL ENQUEUEING");
    
    // READ AND PRINT THE RESULT
    err = clEnqueueReadBuffer(cQ,read_buff,CL_TRUE,0,sizeof(test),&test,0,NULL,NULL);
    check_error(err, "BUFFER READING");
    
    /*for(cl_int i=0; i<24; i+=6)
    {
        printf("%.2f    %.2f    %.2f    %.2f    %.2f    %.2f\n", test[i],test[i+1],test[i+2],test[i+3],test[i+4],test[i+5]);
    }*/

    printf("NUMBER OF LAUNCHED WORK-ITEMS: %d\n",Gx*Gy);
    printf("NUMBER OF LAUNCHED WORK-GROUPS: %d\n",(Gx*Gy)/(Lx*Ly));
    printf("WORK-GROUPS SIZE: (%dx%d)\n",Lx,Ly);

    printf("2D GRID TO 1D GRID:\n");


    /*for(cl_int i=0; i<Gx*Gy; i++)
    {
	    printf("work_item[%d] = %10.6f\n",i,test[i]);
    }*/

    get_3Dcube_file(Nx,Ny,Nz,x0,y0,z0,hx,hy,hz,test,Ntot);
        
    // FREE RESOURCES
    
    clReleaseMemObject(read_buff);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(cQ);
    clReleaseProgram(program);
    clReleaseContext(context);
    
    
    return 0;
}

