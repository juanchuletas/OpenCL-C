//
//  performance.c
//  
//
//  Created by Juan García on 2/26/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PROGRAM_FILE "get_fitness.cl"
#define KERNEL_NAME_FUNC "GetFitness"
#include "OPENCL_HEADERS.h"
#include "ocl_data_structures.h"
//******************************************************
//*** EXTERNAL FUNCTIONS *******************************
extern void CheckError(cl_int err, char *str);
extern void DisplayDeviceInfo(cl_device_id id,cl_device_info param_name);
extern void DisplayPlatformInfo(cl_platform_id id, cl_platform_info param_name,char *request);
extern void GetOpenCLInfo(void);
extern void user_input(int *Nx,int *Ny, int *Nz,float *x0, float  *xN,float *y0,float *yN,float *z0,float *zN,float *hx, float *hy,float *hz,int *Ntot);
extern void AlGenUsrInput(int *N_ind,int *N_color,int *M,int *N, int *N_reg, char filename[256]);
extern void disp_ad_matrix(struct Node **admat,int n_reg);
extern void get_ad_matrix(struct Node **admat,int n_reg,char filename[256]);
extern void ComputeAdjMat(int *admat,int n_reg,char filename[256]);
extern void FillZeroMat(int *mat,int M,int N);
extern void print_matrix( char* desc, int m, int n, int *a);
extern void RandPopulation(int *pop_mat,int N_ind,int N_reg,int color);
// ********************************
void GetComputeUnits(cl_device_id did)
{
    cl_int err;
    cl_uint cu,wi,max_WIDs_dim;
    CheckError(err,"GETTING COMPUTE UNITS");
    err = clGetDeviceInfo(did,CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(cu),&cu,NULL);
    CheckError(err,"GETTING COMPUTE UNITS");
    err = clGetDeviceInfo(did,CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,sizeof(cl_uint),&max_WIDs_dim,NULL);
    size_t *arr = (size_t *)malloc(sizeof(size_t)*CL_DEVICE_MAX_WORK_ITEM_SIZES);
    
    err = clGetDeviceInfo(did,CL_DEVICE_MAX_WORK_ITEM_SIZES,CL_DEVICE_MAX_WORK_ITEM_SIZES,arr,NULL);
    
    printf("NUMBER OF MAX PARALLEL COMPUTE UNITS: %d\n",cu);

    printf("DIMENSIONS: %d\n",max_WIDs_dim);
    for(cl_uint i=0; i<max_WIDs_dim; i++)
    {
        printf("%zu  ",arr[i]);
    }
    printf("\n");
    

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
    CheckError(err,"PROGRAM CREATION");
    
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
int main ()
{
     //USER DATA TYPE OPENCL_STRUCTURES
    struct OPENCL_STRUCTURES opencl;
    cl_uint pIdx,dIdx;
    
    
    // **+ VARIABLES FOR THE USER DATA ***
    int Nx,Ny,Nz,Ntot;
    float x0,xN,y0,yN,z0,zN,hx,hy,hz;
    
    //CALL TO THE FUNCTION THAT ASKS FOR THE USER DATA
    //user_input(&Nx,&Ny,&Nz,&x0,&xN,&y0,&yN,&z0,&zN,&hx,&hy,&hz,&Ntot);
    
    
    //THE FIRST STEP IN OPENCL IS QUERY FOR THE AVAILABLE HARDWARE
    
    GetOpenCLInfo();
    
    //THE ABOVE FUNCTION DISPLAYS THE AVAILABLE RESOURCES AND YOU MUST SELECT THE PLATFORM AND THE DEVICE TO PERFORM YOUR COMPUTATIONS ********
    printf("SELECT THE ID FOR THE PLATFORM\n");
    scanf("%d",&pIdx);
    printf("SELECT THE ID FOR THE DEVICE\n");
    scanf("%d",&dIdx);
    
    printf("THE OPENCL CONTEXT WILL BE CREATED WITH THE FOLLOWING DATA:\n");
    
    //PLATFORM, DEVICE AND CONTEXT DATA STRUCTURES******
    cl_context context;
    cl_int err;
    
    //MEMORY ALLOCATION FOR THE PLATFORM AND DEVICE DATA STRUCTURES *****
    opencl.platform = (cl_platform_id *)malloc(sizeof(cl_device_id)*(pIdx+1));
    opencl.device = (cl_device_id*)malloc(sizeof(cl_device_id)*(dIdx+1));
    // ******************************************************************
    // STORE THE PLATFORM AND DEVICE DATA STRUCTURES*********************
    err = clGetPlatformIDs(pIdx+1,opencl.platform,NULL);
    CheckError(err,"GETTING YOUR SELECTED PLARTFORM\n");
    err = clGetDeviceIDs(opencl.platform[pIdx],CL_DEVICE_TYPE_ALL,dIdx+1,opencl.device,NULL);
    CheckError(err,"GETTING YOUR SELECTED DEVICES\n");
    DisplayPlatformInfo(opencl.platform[pIdx],CL_PLATFORM_VENDOR,"CL_PLATFORM_VENDOR");
    DisplayDeviceInfo(opencl.device[dIdx],CL_DEVICE_TYPE);
    DisplayDeviceInfo(opencl.device[dIdx],CL_DEVICE_VENDOR); 
    DisplayDeviceInfo(opencl.device[dIdx],CL_DEVICE_NAME);
    //THIS FUNCTION SHOWS THE COMPUTE UNITS FOR THE SELECTED DEVICE *************
    GetComputeUnits(opencl.device[dIdx]);
    // ******************************************************************
    // OPENCL CONTEXT CREATION WITH THE PLATFORM AND DEVICES SELECTED BY THE USER ****
    opencl.context = clCreateContext(NULL,1,&opencl.device[dIdx],NULL,NULL,&err);
    CheckError(err,"CREATING THE CONTEXT");
    //********************************************************************
    //
    
    printf("************  STARTING THE GENETIC ALGORITHM  ************\n");
    int N_ind,N_color,M,N,best,N_reg;
    char filename[256];
    AlGenUsrInput(&N_ind,&N_color,&M,&N,&N_reg,filename);
    printf("No of REG = %d\n",N_reg);
    struct Node *admat[N_reg];
    int gsize = N_reg*N_reg;
    int adjmatrix[gsize];
    FillZeroMat(adjmatrix,N_reg,N_reg);
    get_ad_matrix(admat,N_reg,filename);
    disp_ad_matrix(admat,N_reg);
    ComputeAdjMat(adjmatrix,N_reg,filename);
    print_matrix("ADJ MATRIX",N_reg,N_reg,adjmatrix);

    int pop_mat[N_ind*N_reg];
    int fitness[N_ind];
    int fitnesscpu[N_ind];
    RandPopulation(pop_mat,N_ind,N_reg,N_color);
    print_matrix("RAND POP",N_ind,N_reg,pop_mat);


	   int i=0;
	   do
	   {
             int sum=0;
             for(int k=0; k<N_reg; k++)
             {
                   for(int j=0; j<N_reg; j++)
                   {
                           if(adjmatrix[k*N_reg + j]==1)
                           {
                                   if(pop_mat[i*N_reg + k]==pop_mat[i*N_reg + j])
                                   {
                                           sum = sum + 1;

                                   }
                           }
                   }
            }
	    fitnesscpu[i] = sum;
	    i++;
	   }while(i<N_ind);


      
    // PROGRAM AND KERNEL CREATION **************************************
    char files[N_kernels][1080] = {"get_fitness.cl","get_best_fitness.cl"}
    char kernelName[N_kernels][1080] = {"GetFitness","GetFitness"}
    opencl.program1 = BuildProgram(opencl.context,opencl.device[dIdx],files[0]);
    opencl.program2 = BuildProgram(opencl.context,opencl.device[dIdx],files[1]);
    
    opencl.kernel1 = clCreateKernel(opencl.program1,KERNEL_NAME_FUNC,&err);
    CheckError(err,"KERNEL CREATION 1");
    opencl.kernel2 = clCreateKernel(opencl.program2,KERNEL_NAME_FUNC,&err);
    CheckError(err,"KERNEL CREATION 2");
    //WORK-ITEMS DATA *************************
    //
    //
    int Gx=16,Gy=16;
    int Lx=4,Ly=4;
    size_t dim = 2;
    size_t global_offset[] = {0,0};
    size_t global_size[] = {Gx,Gy};
    size_t local_size[]  = {Lx,Ly};

    printf("NUMBER OF WORK-ITEMS: %d\n",Gx*Gy);
    printf("NUMBER OF WORK-GROUPS: %d\n",(Gx*Gy)/(Lx*Ly));
    printf("WORK-GROUPS SIZE: (%dx%d)\n",Lx,Ly);


    //MEMORY OBJECTS

    cl_mem ocl_admat,ocl_fitness,ocl_popmat;

    //CREATE A WRITE ONLY BUFFER TO HOLD THE OUTPUT DATA
    ocl_admat = clCreateBuffer(opencl.context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(adjmatrix),adjmatrix,&err);
    ocl_popmat = clCreateBuffer(opencl.context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(pop_mat),pop_mat,&err);
    ocl_fitness = clCreateBuffer(opencl.context,CL_MEM_WRITE_ONLY,sizeof(fitness),NULL,&err);

    //SET KERNEL ARGS
    err = clSetKernelArg(opencl.kernel1,0,sizeof(cl_mem),&ocl_fitness);
    err |= clSetKernelArg(opencl.kernel1,1,sizeof(cl_mem),&ocl_popmat);
    err |= clSetKernelArg(opencl.kernel1,2,sizeof(cl_mem),&ocl_admat);
    err |= clSetKernelArg(opencl.kernel1,3,sizeof(N_reg),&N_reg);
    err |= clSetKernelArg(opencl.kernel1,4,sizeof(N_reg),&N_ind);

    CheckError(err, "KERNEL 1 SETTING ARGUMENT");

    // CREATING THE COMMAND QUEUE TO SEND INSTRUCTIONS TO THE DEVICE
    opencl.cQ = clCreateCommandQueue(opencl.context,opencl.device[dIdx],0,&err);
    CheckError(err, "COMMAND QUEUE CREATION");

    // ENQUEUE THE KERNEL
    err = clEnqueueNDRangeKernel(opencl.cQ,opencl.kernel,dim,global_offset,global_size,local_size,0,NULL,NULL);
    CheckError(err,"THE KERNEL ENQUEUEING");

     // READ AND PRINT THE RESULT
    err = clEnqueueReadBuffer(opencl.cQ,ocl_fitness,CL_TRUE,0,sizeof(fitness),&fitness,0,NULL,NULL);
    CheckError(err, "BUFFER READING");


   printf("************  STARTING THE GENETIC ALGORITHM  ************\n");
   for(int i=0; i<N_ind; i++)
   {
	   printf("Ind[ %d ]   fitnessGPU = %d    fitnessCPU = %d\n",i,fitness[i],fitnesscpu[i]);
   }
    
    
    free(opencl.device);
    free(opencl.platform);
    clReleaseContext(opencl.context);
    clReleaseProgram(opencl.program);
    clReleaseKernel(opencl.kernel);
    
    
    return 0;
    
}
