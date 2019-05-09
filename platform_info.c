//
//  platform_info.c
//
// THIS OPENCL CODE ALLOWS TO ACCESS THE PLATFORMS
// INFORMATION IN OUR SYSTEM
//
//  Created by Juan García on 1/14/19.
//

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#define CL_SILENCE_DEPRECATION
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

void display_platform_info(cl_platform_id id, cl_platform_info param_name, const char* in_request)
{
    cl_int err=0;
    size_t param_size = 0;
    err = clGetPlatformInfo(id,param_name,0,NULL,&param_size);
    char *out_info = (char *)malloc(sizeof(char)*param_size);
    err = clGetPlatformInfo(id,param_name,param_size,out_info,NULL);
    if(err != CL_SUCCESS)
    {
        perror("NO PLATFORMS FOUND");
        return;
        
    }
    printf("%s: %s\n",in_request,out_info);
    free(out_info);

}
int main (void)
{
    //OPENCL DATA STRUCTURES
    cl_platform_id *platforms;
    
    //OPENCL SCALAR DATA TYPES
    cl_uint n_platforms;
    cl_int err;
    
    // GET THE NUMBER OF PLATFOTMS
    //NOTE: FOR EACH VENDOR'S SDK INSTALLED ON THE COMPUTER THE
    //NUMBER OF PLATFORMS WILL INCREASE
    // IF WE HAVE INSTALLED BOTH NVIDIA AND AMD SDKs THE NUMBER OF PLATFORMS
    //WILL BE EQUALS TO 2
    err = clGetPlatformIDs(0,NULL,&n_platforms);
    if(err != CL_SUCCESS)
    {
        perror("UNABLE TO FIND ANY OPENCL PLATFORM");
        exit(1);
    }
    //ALLOCATE MEMORY FOR THE NUMBER OF INSTALLED PLATFORMS
    platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id)*n_platforms);
    printf("NUMBER OF OPENCL PLATFOMRS %d\n",n_platforms);
    
    err = clGetPlatformIDs(n_platforms,platforms,NULL);
    if(err != CL_SUCCESS)
    {
        perror("UNABLE TO FIND ANY OPENCL PLATFORM");
        exit(1);
    }
    // NOTE THAT IN THE display_platform_info WE CALL THE clPlatformInfo TWICE
    // BECAUSE THE FIRST ONE IS TO CREATE THE TEMPORARY DATA STRUCTURES ON THE STACK
    // AND THE SECOND ONE STORES THE RETURNED INFO
    for(cl_uint i=0; i<n_platforms; i++)
    {
        display_platform_info(platforms[i],CL_PLATFORM_PROFILE,"CL_PLATFORM_PROFILE");
        display_platform_info(platforms[i],CL_PLATFORM_VERSION,"CL_PLATFORM_VERSION");
        display_platform_info(platforms[i],CL_PLATFORM_NAME,"CL_PLATFORM_NAME");
        display_platform_info(platforms[i],CL_PLATFORM_VENDOR,"CL_PLATFORM_VENDOR");
        display_platform_info(platforms[i],CL_PLATFORM_EXTENSIONS,"CL_PLATFORM_EXTENSION");
    }
    free(platforms);
    return 0;
}
