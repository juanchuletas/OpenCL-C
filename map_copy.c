//
//  map_copy.c
//  
//
//  Created by Juan García on 1/17/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define PROGRAM_FILE "blank.cl"
#define KERNEL_FUNC "blank"

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
//FUNCTION TO GET THE INSTALLED DEVICES
cl_device_id create_device()
{
    cl_platform_id platform;
    cl_device_id dev;
    cl_int
    
    err  = clGetPlatformIDs(1,&platform,NULL);
    check_error(err,"CREATING PLATFORM");
    
    //ACCESING A DEVICE
    err = clGetDeviceIDs(platform,CL_DEVICE_TYPE_GPU,1,&dev,NULL);
    if(err==CL_DEVICE_NOT_FOUND)
    {
        err = clGetDeviceIDs(platform,CL_DEVICE_TYPE_CPU,1,&dev,NULL);
    }
    check_error(err,"ALLOCATING DEVICE DATA STRUCTURE");
    
    return dev;
}
cl_program build_program(cl_context cntx, cl_device_id dev, const char *file_name)
{
    cl_program program;
    FILE *program_handle;
    char *program_buffer, *programlog;
    size_t program_size, log_size;
    cl_int err;
    
    program_handle = fopen(file_name,"r");
    if(program_handle==NULL)
    {
        perror("FILE NOT FOUND\n");
        exit(1);
    }
    fseek(program_handle,0,SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char *)malloc(sizeof(char)*program_size+1);
    program_buffer[program_size]='\0';
    fread(program_buffer,sizeof(char),program_size,program_handle);
    fclose(program_handle);
    
    //CREATE THE PROGRAM FROM THE BUFFER
    program=clCreateProgramWithSource(cntx,1,(const char**)&program_buffer,&program_size,&err);
    check_error(err,"CREATING PROGRAM");
    free(program_buffer);
    
    //AFTER OF CREATING THE PROGRAM WE NEED TO BUILD IT
    err = clBuildProgram(program,0,NULL,NULL,NULL,NULL);
    if(err<0)
    {
        clGetProgramBuildInfo(program,dev,CL_PROGRAM_BUILD_LOG,0,NULL,&log_size);
        programlog = (char *)malloc(sizeof(char)*log_size+1);
        programlog[log_size]='\0';
        clGetProgramBuildInfo(program,dev,CL_PROGRAM_BUILD_LOG,log_size+1,programlog,NULL);
        printf("%s\n",programlog);
        free(programlog);
        exit(1);
    }
    return program;
}
int main (void)
{
    //OPENCL DATA STRUCTURES
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_int err;
    
    
    //DATA BUFFERS
    float data_one[100],data_two[100],res[100];
    cl_mem buffer_one, buffer_two;
    void *map_mem;
    
    for(cl_int i=0; i<100; i++)
    {
        data_one[i] = 1.0f*i;
        data_two[i] = -1.0f*i;
        res[i] = 0.0f;
    }
    
    //GET THE DEVICE AND CREATE THE CONTEXT
    device = create_device();
    context = clCreateContext(NULL,1,&device,NULL,NULL,&err);
    check_error(err,"CREATING THE CONTEXT");
    
    //BUILD THE PROGRAM AND CREATE THE KERNEL
    program = build_program(context,device,PROGRAM_FILE);
    kernel = clCreateKernel(program,KERNEL_FUNC,&err);
    check_error(err,"CREATING THE KERNEL");
    
    //CREATING THE BUFFERS
    buffer_one = clCreateBuffer(context,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,sizeof(data_one),data_one,&err);
    check_error(err,"CREATING BUFFER ONE");
    
    buffer_two = clCreateBuffer(context,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,sizeof(data_two),data_two,NULL);
    check_error(err,"CREATING BUFFER TWO");
    
    err = clSetKernelArg(kernel,0,sizeof(cl_mem),&buffer_one);
    err |= clSetKernelArg(kernel,1,sizeof(cl_mem),&buffer_two);
    
    check_error(err,"SETTING THE BUFFERS AS THE KERNELS ARGUMENTS");
    
    //CREATE THE COMMAND QUEUE TO ESTABLISH THE COMMUNICATION
    
    queue = clCreateCommandQueue(context,device,0,&err);
    check_error(err,"CREATING THE COMMAND QUEUE");
    
    //ENQUEUE THE KERNEL
    err = clEnqueueTask(queue,kernel,0,NULL,NULL);
    check_error(err,"ENQUEUEING THE KERNEL");
    
    //ENQUEUE COMMAND TO COPY BUFFER ONE TO BUFFER TWO
    err = clEnqueueCopyBuffer(queue,buffer_one,buffer_two,0,0,sizeof(data_one),0,NULL,NULL);
    
    //ENQUEUE COMMAND TO MAP BUFFER TWO TO HOST MEMORY
    map_mem = clEnqueueMapBuffer(queue,buffer_two,CL_TRUE,CL_MAP_READ,0,sizeof(data_two),0,NULL,NULL,&err);
    check_error(err,"MAPPING THE BUFFER TO HOST MEMORY");
    
    //TRANSFER MEMORY
    memcpy(res,map_mem,sizeof(data_two));
    
    //UNMAP MEMOERY
    err = clEnqueueUnmapMemObject(queue,buffer_two,map_mem,0,NULL,NULL);
    
    check_error(err,"UNMAPPING THE BUFFER");
    
    //DISPLAY THE UPDATED BUFFER;
    //for(cl_int i=0; i<10; i++)
    //{
     for(cl_int i=0; i<10; i++) {
      for(cl_int j=0; j<10; j++) {
         printf("%6.1f", res[j+i*10]);
      }
      printf("\n");
   }

    //}
    
    //DEALOCATE RESOURCES
    clReleaseMemObject(buffer_one);
    clReleaseMemObject(buffer_two);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
}
