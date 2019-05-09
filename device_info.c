//
//  device_info.c
// device_info.c
//  
//  PROGRAM TO LIST THE INFORMATION OF ALL
//  THE DEVICES INSTALLED IN THE OPENCL API 
//
//  DEVELOPER: Juan José García
//
//  Created by Juan García on 1/15/19.
//  On the Xcode IDE for Apple Inc.

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
void display_device_details(cl_device_id id, cl_device_info param_name)
{
    cl_int err = 0;
    size_t param_size = 0;
    
    // FIRST CALL TO OBTAIN THE SIZE OF THE PARAMETER
    err  = clGetDeviceInfo(id, param_name,0,NULL,&param_size);
    if(err!=CL_SUCCESS)
    {
        perror("NO DEVICE INFO");
        return;
    }
    // EASY SWITCH-CASE CONDITION FOR THE param_name VALUE
    switch(param_name)
    {
        case CL_DEVICE_TYPE:
        {
            //QUERY FOR THE DEVICE TYPE: CPUs OR GPUs
            cl_device_type *dev_type = (cl_device_type*)malloc(sizeof(cl_device_type)*param_size);
            err = clGetDeviceInfo(id, param_name, param_size, dev_type, NULL);
            if(err!=CL_SUCCESS)
            {
                perror("NO DEVICE INFO");
                return;
            }
            //SWITCH-CASE STATEMENT FOR THE IDENTIFIED DEVICES
            switch(*dev_type) //REMEMBER THAT dev_type IS A DATA STRUCTURE
            {
                case CL_DEVICE_TYPE_CPU: printf("CPU AVAILABLE\n");break;
                case CL_DEVICE_TYPE_GPU: printf("GPU AVAILABLE\n");break;
                case CL_DEVICE_TYPE_ACCELERATOR: printf("ACCELERATOR AVAILABLE\n");break;
                case CL_DEVICE_TYPE_DEFAULT: printf("DEFAULT DEVICE AVAILABLE\n");break;
            }
            free(dev_type);
        }break;
        case CL_DEVICE_VENDOR_ID: //IDENTIFIES THE VENDOR
        case CL_DEVICE_MAX_COMPUTE_UNITS: // IDENTIFIES THE NUM OF COMPUTE UNITS
        case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:
        {
            cl_uint *ret = (cl_uint *)malloc(sizeof(cl_uint)*param_size);
            err = clGetDeviceInfo(id,param_name,param_size,ret,NULL);
            if(err!=CL_SUCCESS)
            {
                perror("NO DEVICE INFO");
                return;
            }
            switch(param_name)
            {
                case CL_DEVICE_VENDOR_ID: printf("\tVENDOR ID: 0x%x\n",*ret);break;
                case CL_DEVICE_MAX_COMPUTE_UNITS: printf("\tMAXIMUM NUMBER OF PARALLAEL COMPUTE UNITS: %d\n", *ret);break;
                case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: printf("\tMAXIMUM DIMENSIONS FOR THE GLOBAL/LOCAL WORK ITEM IDs: %d\n",*ret);break;
            }
            free(ret);
        }break;
        case CL_DEVICE_MAX_WORK_ITEM_SIZES:
        {
            cl_uint max_WIDs_dim;
            size_t *ret = (size_t *)malloc(sizeof(size_t)*param_name);
            err = clGetDeviceInfo(id,param_name,param_size,ret,NULL);
            err = clGetDeviceInfo(id,CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,sizeof(cl_uint), &max_WIDs_dim,NULL);
            if(err!=CL_SUCCESS)
            {
                perror("NO DEVICE INFO");
                return;
            }
            printf("\tMAXIMUM NUMBER OF WORK-ITEMS IN EACH DIMENSION: (");
            for(cl_int i=0; i<max_WIDs_dim; i++)
            {
                printf("%zu ",ret[i]);
            }
            printf(" )\n");
            free(ret);
        }break;
        case CL_DEVICE_MAX_WORK_GROUP_SIZE:
        {
            size_t *ret = (size_t*)malloc(sizeof(size_t)*param_size);
            err  = clGetDeviceInfo(id, param_name, param_size, ret,NULL);
            if(err!=CL_SUCCESS)
            {
                perror("NO DEVICE INFO");
                return;
            }
            printf("\tMAXIMUM NUMBER OF WORK-ITEMS IN A WORK-GROUP: %zu\n",*ret);
            free(ret);
        }break;
        case CL_DEVICE_NAME :
        case CL_DEVICE_VENDOR :
        {
            char data[48]; //ENOUGH FOR THE DATA WE WANT TO KNOW
            err = clGetDeviceInfo(id, param_name,param_size,data,NULL);
            if(err!=CL_SUCCESS)
            {
                perror("NO DEVICE INFO");
                return;
            }
            switch(param_name)
            {
                 case CL_DEVICE_NAME: printf("\tDEVICE NAME IS %s\n",data);break;
                 case CL_DEVICE_VENDOR: printf("\tDEVICE_VENDOR IS %s\n",data);break;
                
            }
        }break;
        case CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE: //CACHE LINE MEMORY SIZE
        {
            cl_uint *size = (cl_uint *)malloc(sizeof(cl_uint)*param_size);
            err = clGetDeviceInfo(id,param_name,param_size,size,NULL);
            if(err!=CL_SUCCESS)
            {
                perror("NO DEVICE INFO");
                return;
            }
            printf("\tDEVICE GLOBAL CACHE LINE SIZE: %d BYTES\n",(*size));
        }break;
        case CL_DEVICE_GLOBAL_MEM_SIZE:
        case CL_DEVICE_MAX_MEM_ALLOC_SIZE:
        {
            cl_ulong *size = (cl_ulong*)malloc(sizeof(cl_ulong)*param_size);
            err = clGetDeviceInfo(id,param_name,param_size,size,NULL);
            if(err!=CL_SUCCESS)
            {
                perror("NO DEVICE NAME/VENDOR INFO");
                return;
            }
            switch (param_name)
            {
                case CL_DEVICE_GLOBAL_MEM_SIZE: printf("\tDEVICE GLOBAL MEM: %llu MB\n",(*size)>>20);break;
                case CL_DEVICE_MAX_MEM_ALLOC_SIZE: printf("\tDEVICE MAX MEMORY ALLOCATION %llu MB\n",(*size)>>20);break;
            }
            free(size);
        }break;
    }// END OF THE MASTER SWITCH
}
void display_device_info(cl_platform_id id, cl_device_type dev_type)
{
    //DEVICE TYPES
    cl_int err  = 0;
    cl_uint n_dev;
    // GET THE NUMBER OF DEVICES CONECTED TO THE PLATFORM
    err = clGetDeviceIDs(id,dev_type,0,NULL,&n_dev);
    if(err!=CL_SUCCESS)
    {
        perror("NO DEVICES CONECTED TO THE PLATFORM");
        exit(1);
    }
    cl_device_id *devices = (cl_device_id *)malloc(sizeof(cl_device_id)*n_dev);
    
    // LOAD ALL THE INFORMATION ABOUT THE AVAILABLE DEVICES INTO THE devices DATA
    // STRUCTTURE
    err = clGetDeviceIDs(id,dev_type,n_dev,devices,NULL);
    if(err!=CL_SUCCESS)
    {
        perror("NO DEVICES CONECTED TO THE PLATFORM");
        exit(1);
    }
    printf("NUMBER OF DEVICES IN THIS MACHINE: %d\n", n_dev);
    printf("SIZE OF DEVICE ARRAY: %lu\n",sizeof(devices));

    for(cl_int i=0; i<n_dev; i++)
    {
        display_device_details(devices[i],CL_DEVICE_TYPE);
        display_device_details(devices[i],CL_DEVICE_NAME);
        display_device_details(devices[i],CL_DEVICE_VENDOR);
        display_device_details(devices[i],CL_DEVICE_VENDOR_ID);
        display_device_details(devices[i],CL_DEVICE_MAX_MEM_ALLOC_SIZE);
        display_device_details(devices[i],CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
        display_device_details(devices[i],CL_DEVICE_GLOBAL_MEM_SIZE);
        display_device_details(devices[i],CL_DEVICE_MAX_COMPUTE_UNITS);
        display_device_details(devices[i],CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
        display_device_details(devices[i],CL_DEVICE_MAX_WORK_ITEM_SIZES);
        display_device_details(devices[i],CL_DEVICE_MAX_WORK_GROUP_SIZE);
    }
    free(devices);
}
int main (void)
{
    // OPENCL DATA STRUCTURES
    cl_platform_id *platforms;
    
    // SCALAR DATA TYPES
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
    // THE FIRST ONE IS FOR CREATE THE TEMPORARY DATA STRUCTURES ON THE STACK
    // AND THE SECOND ONE STORES IS FOR OBTAIN THE RETURNED INFO
    for(cl_uint i=0; i<n_platforms; i++)
    {
        display_platform_info(platforms[i],CL_PLATFORM_PROFILE,"CL_PLATFORM_PROFILE");
        display_platform_info(platforms[i],CL_PLATFORM_VERSION,"CL_PLATFORM_VERSION");
        display_platform_info(platforms[i],CL_PLATFORM_NAME,"CL_PLATFORM_NAME");
        display_platform_info(platforms[i],CL_PLATFORM_VENDOR,"CL_PLATFORM_VENDOR");
        display_platform_info(platforms[i],CL_PLATFORM_EXTENSIONS,"CL_PLATFORM_EXTENSION");
        display_device_info(platforms[i],CL_DEVICE_TYPE_ALL);
    }
    free(platforms);

}
