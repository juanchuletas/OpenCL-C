
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "OPENCL_HEADERS.h"


extern void CheckError(cl_int err, char *str);
extern void DisplayDeviceInfo(cl_device_id id,cl_device_info param_name);
extern void DisplayPlatformInfo(cl_platform_id id, cl_platform_info param_name,char *request);

cl_device_id *GetDeviceInPlatform(cl_platform_id id,cl_device_type dev_type,cl_uint *ndev)
{
    cl_int err;
    cl_uint n_dev;
    //GET THE NUMBER OF DEVICES CONECTED ON THE PLATFORM
    err = clGetDeviceIDs(id,dev_type,0,NULL,&n_dev);
    CheckError(err,"GETTING THE DEVICES IN PLATFORM");
    cl_device_id *device = (cl_device_id *)malloc(sizeof(cl_device_id)*n_dev);
    err = clGetDeviceIDs(id,dev_type,n_dev,device,NULL);
    CheckError(err,"ALLOCATING THE DEVICES IN THE DATA STRUCTURE");
    printf("NUMBER OF DEVICES IN THIS MACHINE: %d\n",n_dev);
    *ndev=n_dev;
    
    return device;
    
    
}
void GetOpenCLInfo(void)
{
    //FUNCTION TO GET THE OPENCL INFORMATION OF THE COMPUTER SYSTEM
    printf("************** OPENCL INFORMATION *************************************\n");
    printf("************ AVAILABLE IN YOUR SYSTEM *********************************\n");
    
    cl_platform_id *platform;
    cl_uint n_platforms,NDEV,pindx;
    cl_device_id *device;
    cl_uint nplat;
    cl_int err;
    //FIRST CALL IS FOR ASKING THE NUMBER OF PLATFORMS AVAILABLE:
    err = clGetPlatformIDs(0,NULL,&nplat);
    CheckError(err, "ASKING FOR PLATFORMS");
    //MEMORY ALLOCATION FOR THE PLATFOFORMS
    platform = (cl_platform_id *)malloc(sizeof(cl_platform_id)*nplat);
    
    printf("NUMBER OF OPENCL PLATFORMS IN YOUR COMPUTER: %d\n",nplat);

    //DATA STRUCTURE CREATION FOR THE PLATFORMS
    err = clGetPlatformIDs(nplat,platform,NULL);
    CheckError(err,"ALLOCATING THE PLATFORMS");
    for(cl_uint i=0; i<nplat; i++)
    {
        DisplayPlatformInfo(platform[i],CL_PLATFORM_VENDOR,"CL_PLATFORM_VENDOR");
        device = GetDeviceInPlatform(platform[i],CL_DEVICE_TYPE_ALL,&NDEV);
        printf("NUMBER OF DEVICES PER PLATFORM: %d\n",NDEV);
        for(cl_uint j=0; j<NDEV; j++)
        {
            printf("************************************************************************\n");
            printf("PLATFORM %d DEVICE %d\n",i,j);
            
            DisplayDeviceInfo(device[j],CL_DEVICE_TYPE);
            DisplayDeviceInfo(device[j],CL_DEVICE_VENDOR);
            DisplayDeviceInfo(device[j],CL_DEVICE_NAME);
            DisplayDeviceInfo(device[j],CL_DEVICE_MAX_COMPUTE_UNITS);
            DisplayDeviceInfo(device[j],CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
            DisplayDeviceInfo(device[j],CL_DEVICE_MAX_WORK_ITEM_SIZES);
            DisplayDeviceInfo(device[j],CL_DEVICE_MAX_WORK_GROUP_SIZE);
            printf("************************************************************************\n");
            
        }
        
    }
 
    free(platform);
    free(device);

}
