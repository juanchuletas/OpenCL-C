//
//  performance.c
//  
//
//  Created by Juan García on 2/26/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PROGRAM_FILE "fz_function.cl"
#define KERNEL_NAME_FUNC "fz_function"
#include "OPENCL_HEADERS.h"
// **** STRUCTURES TO STORE ALL THE OPENCL DATA STRUCTURES ******
struct OPENCL_STRUCTURES
{
    cl_platform_id *platform;
    cl_device_id *device;
    cl_context context;
    cl_command_queue cQ;
    cl_program program;
    cl_kernel kernel;
};
//****************************************************
// STRUCTURES TO STORE THE OPENCL MEMORY OBJECTS ******
struct OPENCL_BUFFERS
{
    // MEMORY OBJECTS *********

    cl_mem read_buff,buff_step,buff_points;
};
//******************************************************
//*** EXTERNAL FUNCTIONS *******************************
extern void CheckError(cl_int err, char *str);
extern void DisplayDeviceInfo(cl_device_id id,cl_device_info param_name);
extern void DisplayPlatformInfo(cl_platform_id id, cl_platform_info param_name,char *request);
extern void GetOpenCLInfo(void);
extern void user_input(int *Nx,int *Ny, int *Nz,float *x0, float  *xN,float *y0,float *yN,float *z0,float *zN,float *hx, float *hy,float *hz,int *Ntot);

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
    //******************************************************************
    
    float step[] = {hx,hy,hz};
    float point0[] ={x0,y0,z0};
    
    
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
    
    // PROGRAM AND KERNEL CREATION ***************************************
    opencl.program = BuildProgram(opencl.context,opencl.device[dIdx],PROGRAM_FILE);
    
    opencl.kernel = clCreateKernel(opencl.program,KERNEL_NAME_FUNC,&err);
    CheckError(err,"KERNEL CREATION");
    //WORK-ITEMS DATA *************************
    /*size_t dim = 2;
    size_t global_offset[] = {0,0};
    size_t global_size[] = {Gx,Gy};
    size_t local_size[]  = {Lx,Ly};*/
    

    
    
    
    
    free(opencl.device);
    free(opencl.platform);
    clReleaseContext(opencl.context);
    clReleaseProgram(opencl.program);
    clReleaseKernel(opencl.kernel);
    
    
    return 0;
    
}
