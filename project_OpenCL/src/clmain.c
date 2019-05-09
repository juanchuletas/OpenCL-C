#include<stdio.h>
#include <stdlib.h>
#include <math.h>


#include "OPENCL_HEADERS"

typedef struct OPENCL_STRUCTURES
{
    cl_platform_id *platform;
    cl_device_id *device;
    cl_context context;
    cl_command_queue *cQ;
    cl_program program;
    cl_kernel kernel;
    
}opencl_struct;

typedef struct OPENCL_BUFFERS
{
    cl_mem read_buff, buff_step, buff_points;
}opencl_buff;



int main (void)
{
    
    GetOpenCLInfo();
    //USER DATA TYPE OPENCL STRUCTURES------------------------------
    opencl_struct ocl;
    
    clGetPletfotmIDs    
    
}
