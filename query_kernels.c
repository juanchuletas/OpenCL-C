//
//  query_kernels.c
//  
//
//  Created by Juan García on 1/16/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#define CL_SILENCE_DEPRECATION
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

void load_program(const char **in_files,size_t length,char **buffer, size_t *sizes)
{
    for(size_t i=0; i<length; i++)
    {
        FILE * program_handle = fopen(in_files[i],"r");
        if(input==0)
        {
            perror("FILE NOT FOUND");
            exit(1);
        }
        fseek(program_handle,0,SEEK_END);
        sizes[i] = ftell(program_handle);
        buffer[i] = (char *)malloc(sizeof(char)*sizes[i]);
        buffer[i][sizes[i]] = '\0';
        fread(buffer[i],sizeof(char),sizes[i],program_handle);
    }
}
int main (void)
{
    //OPENCL DATA STRUCTURS
    cl_platform_id *platforms;
    cl_program program;
    cl_device_id device;
}
