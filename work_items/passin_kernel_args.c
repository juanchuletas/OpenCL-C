//
//  id_checking.c
//  
//
//  Created by Juan García on 1/24/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PROGRAM_FILE "passin_kernel_args.cl"
#define KERNEL_NAME_FUNC "passing"

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

// FUNTCTION TO SET THE DEVICES
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
void store_data(int *send)
{
	for(int i=0; i<16;i++)
	{
		send[i] = i*i;
	}
}
int main(void)
{
    cl_device_id device;
    cl_context context;
    cl_command_queue cQ;
    cl_program program;
    cl_kernel kernel;
    cl_int err;
    int test[16];
    int send[16];


    store_data(send); // FILL THE ARRAY

    //DATA FOR THE WORK-ITEMS!!!
    
    size_t dim = 2;
    size_t global_offset[] = {0,0};
    size_t global_size[] = {4,4};
    size_t local_size[] = {2,2};

    //MEMORY OBJECTS
    //
    cl_mem read_buff,input_data;
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
    input_data = clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(send),send,&err);
    check_error(err,"BUFFER CREATION");
    
    //CREATE KERNEL ARGUMENT
    err = clSetKernelArg(kernel,0,sizeof(cl_mem),&read_buff);
    err = clSetKernelArg(kernel,1,sizeof(cl_mem),&input_data);
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

    printf("NUMBER OF LAUNCHED WORK-ITEMS: %lu\n",global_size[0]*global_size[1]);
    printf("NUMBER OF LAUNCHED WORK-GROUPS: %lu\n",local_size[1]*local_size[0]);
    printf("WORK-GROUPS SIZE: (%lux%lu)\n",local_size[1],local_size[0]);

    printf("2D GRID TO 1D GRID:\n");


    for(cl_int i=0; i<16; i++)
    {
	    printf("global_index[%d] = %d\n",i,test[i]);
    }
        
    // FREE RESOURCES
    
    clReleaseMemObject(read_buff);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(cQ);
    clReleaseProgram(program);
    clReleaseContext(context);
    
    
    return 0;
}

