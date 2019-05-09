//
//  use_of_buffer.c
//  
//
//  Created by Juan García on 1/20/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "structures.h"
#ifdef __APPLE__
#define CL_SILENCE_DEPRECATION
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#define DATA_SIZE 1048576
//CHECK THE ERROR IN EACH CALL OF A OPENCL API
void check_error(cl_int err, char *str)
{
    if(err<0)
    {
        printf("SORRY,THERE IS AN ERROR IN %s\n",str);
        exit(1);
    }
}


//TEST FOR VALID VALUES
int tes_values(usr_data* to, usr_data* from)
{
    for(int i=0; i<DATA_SIZE; i++)
    {
        if(to[i].w != from[i].w)
        {
            return 0;
        }
    }
    return 1;
}

void loadProgramFromSource(const char** files,size_t lenght,char** buffer, size_t* sizes)
{
    //READS EACH SOURCE FILE .cl AND STORE THE CONTENTS INTO A TEMPORARY DATASTORE
    for(size_t i=0; i<lenght; i++)
    {
        FILE* program_handle = fopen(files[i],"r");
        if(program_handle==NULL)
        {
            perror("FILE NOT FOUND");
            exit(1);
        }
        fseek(program_handle,0,SEEK_END);
        sizes[i] = ftell(program_handle);
        rewind(program_handle); //RESET THE FILE POINTER SO THAT fread READS FROM THE FRONT
        buffer[i] = (char *)malloc(sizes[i]+1);
        buffer[i][sizes[i]] = '\0';
        fread(buffer[i],sizeof(char),sizes[i],program_handle);
        fclose(program_handle);
    }
}
int main (void)
{
    //OPENCL DATA STRUCTURES
    cl_platform_id *platforms;
    cl_program program;
    cl_device_id device;
    cl_context context;
    
    //SCALAR DATA TYPES
    cl_uint n_platforms;
    cl_int err;
    
    //USER DATA TYPE, DATA STRUCTURES
    usr_data *atm_in = (usr_data *)malloc(sizeof(usr_data)*DATA_SIZE);
    usr_data *atm_out = (usr_data *)malloc(sizeof(usr_data)*DATA_SIZE);
    
    for(int i=0; i<DATA_SIZE; i++)
    {
        atm_in[i].x = i;
        atm_in[i].y = i;
        atm_in[i].z = i;
        atm_in[i].w = 3*i;
    }

    
    //REQUEST FOR THE NUMBER OF AVAILABLE PALTFORMS
    
    err = clGetPlatformIDs(0,NULL,&n_platforms);
    check_error(err,"GETTING THE NUMBER OF PLATFORMS");
    platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id)*n_platforms);
    
    printf("NUMBER OF PLATFORMS FOUND: %d\n",n_platforms);
    
    err = clGetPlatformIDs(n_platforms,platforms,NULL);
    check_error(err,"GETTING THE PLATOFORMS INSTALLED");
    
    //SEARCH FOR A COMPUTE DEVICE (CPU OR GPU) THROUGH THE INSTALLED PLATFORMS
    // THEN BUILD AN OPENCL PROGRAM AND DON'T RUN IT
    
    for(cl_uint i=0; i<n_platforms; i++)
    {
        err = clGetDeviceIDs(platforms[i],CL_DEVICE_TYPE_GPU,1,&device,NULL);
        if(err!=CL_SUCCESS)
        {
            err = clGetDeviceIDs(platforms[i],CL_DEVICE_TYPE_CPU,1,&device,NULL);
        }
        check_error(err,"LOCATING A COMPUTE DEVICE");
        
        //CREATE THE CONTEXT FOT THE DEVICE AND THE PLATFORMS
        context  = clCreateContext(NULL, 1, &device,NULL,NULL,&err);
        check_error(err,"CREATING THE CONTEXT");
        
        
        //LOAD THE SOURCE FILES INTO MEMORY TEMPORARY DATASTORES
        const char *file_names[] = {"test.cl"}; //<-- KERNEL SOURCE
        const int N_OF_FILES = 1;
        char *buffer[N_OF_FILES];
        size_t sizes[N_OF_FILES];
        
        //CALL THE LOAD PROGRAM FROM SOURCE FUNCTION ABOVE
        loadProgramFromSource(file_names,N_OF_FILES,buffer,sizes);
        
        //NOW WE CAN CREATE THE OPENCL PROGRAM OBJECT
        
        program = clCreateProgramWithSource(context,N_OF_FILES,(const char**)buffer,sizes,&err);
        check_error(err,"CREATING THE PROGRAM OBJECT");
        
        //ONCE THE PROGRAM OBJECT WAS CREATED, WE CAN BUILD IT
        char *program_log;
        size_t log_size;
        err = clBuildProgram(program,1,&device,NULL,NULL,NULL);
        if(err!=CL_SUCCESS)
        {
            //IF THERE'S AN ERROR IN BUILDING THE PROGRAM, DUMP THE LOG
            clGetProgramBuildInfo(program,device,CL_PROGRAM_BUILD_LOG,0,NULL,&log_size);
            program_log = (char *)malloc(log_size+1);
            program_log[log_size] = '\0';
            clGetProgramBuildInfo(program,device,CL_PROGRAM_BUILD_LOG,log_size+1,program_log,NULL);
            printf("\n********* ERROR *****\n\n%s\n ***************\n",program_log);
            free(program_log);
            exit(1);
        }
        //QUERY THE PROGRAM AS TO HOW MANY KERNELS WERE DETECTED
        cl_uint n_kernels;
        err = clCreateKernelsInProgram(program,0,NULL,&n_kernels);
        check_error(err,"COUNTING THE NUMBER OF KERNELS FROM PROGRAM");
        cl_kernel *kernels = (cl_kernel *)malloc(sizeof(cl_kernel)*n_kernels);
        err = clCreateKernelsInProgram(program,n_kernels,kernels,NULL);
        for(cl_int i=0; i<n_kernels; i++)
        {
            char kernel_name[40];
            cl_uint arGcnt;
            clGetKernelInfo(kernels[i],CL_KERNEL_FUNCTION_NAME,sizeof(kernel_name),kernel_name,NULL);
            clGetKernelInfo(kernels[i],CL_KERNEL_NUM_ARGS, sizeof(arGcnt),&arGcnt,NULL);
            printf("KERNEL NAME: %s WITH ARGUMENTS: %d\n",kernel_name,arGcnt);
            
            //CREATE THE COMMAND QUEUE TO ENQUEUE THE KERNEL
            cl_command_queue cmdQ = clCreateCommandQueue(context,device,0,&err);
            check_error(err,"CREATING THE COMMAND QUEUE");
            
            //CREATE THE MEMORY OBJECT: BUFFER OBJECT
            cl_mem usr_object = clCreateBuffer(context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(usr_data)*DATA_SIZE,atm_in,&err);
            check_error(err,"CREATING THE BUFFER OBJECT");
            
            //OPENCL HAS TO KNOW THAT THE KERNEL IS SUPPOSE TO RECEIVE AN ARGUMENT
            err = clSetKernelArg(kernels[i],0,sizeof(cl_mem),&usr_object);
            check_error(err,"SETTING THE ARGUMENTS TO THE KERNELS");
            
            printf("THE TASK HAS BEEN SUCCESFULY ENQUEUED\n");
            //ENQUEUE THE READ-BACJ FROM THE DEVICE TO THE ACTUAL HOST
            
            err = clEnqueueReadBuffer(cmdQ,usr_object,CL_TRUE,0,sizeof(usr_data)*DATA_SIZE,atm_out,0,NULL,NULL);
            check_error(err,"READ BACK THE ENQUEUE");
            
            //CHECK THE DATA TRANSFER
            if(tes_values(atm_in,atm_out))
            {
                printf("SUCCESFULLY TRANFSER\n");
            }
            else
            {
                printf("WRONG TRANSFER\n");
            }
            //RELEASE THE COMMAND QUEUE
            clReleaseCommandQueue(cmdQ);
            clReleaseMemObject(usr_object);
        }
        
        //RELEASE ALL STUFF
        for(cl_uint i=0; i<n_kernels; i++)
        {
            clReleaseKernel(kernels[i]);
        }
        for(cl_int i=0; i<N_OF_FILES; i++)
        {
            free(buffer[i]);
        }
        clReleaseProgram(program);
        clReleaseContext(context);
        
        
    }
    
    free(atm_in);
    free(atm_out);
}
