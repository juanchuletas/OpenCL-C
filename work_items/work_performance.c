//
//  work_performance.c
//  
//
//  Created by Juan García on 2/14/19.
//

#include <stdio.h>
#include <stdlib.h>

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

void DisplayPlatformInfo(cl_platform_id id,cl_platform_info param_name,char *request)
{
	cl_int err=0;
	size_t param_size = 0;
	err = clGetPlatformInfo(id,param_name,0,NULL,&param_size);
	char *out_info = (char *)malloc(sizeof(char)*param_size);
	err = clGetPlatformInfo(id,param_name,param_size,out_info,NULL);
	check_error(err,"GETTING THE PLATFORM INFO");
	printf("PLATFORM VENDOR NAME: %s\n",out_info);

	free(out_info);
}

cl_device_id *GetDeviceInPlatform(cl_platform_id id,cl_device_type dev_type,cl_uint *ndev)
{
	cl_int err;
	cl_uint n_dev;
	//GET THE NUMBER OF DEVICES CONECTED ON THE PLATFORM
	err = clGetDeviceIDs(id,dev_type,0,NULL,&n_dev);
	check_error(err,"GETTING THE DEVICES IN PLATFORM");
	cl_device_id *device = (cl_device_id *)malloc(sizeof(cl_device_id)*n_dev);
	err = clGetDeviceIDs(id,dev_type,n_dev,device,NULL);
	check_error(err,"ALLOCATING THE DEVICES IN THE DATA STRUCTURE");
	printf("NUMBER OF DEVICES IN THIS MACHINE: %d\n",n_dev);
	*ndev=n_dev;

	return device;


}
void DisplayDeviceInfo(cl_device_id id, cl_device_info param_name)
{
    size_t param_size = 0;
    cl_int err = 0;



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
int main (void)
{
    //OPENCL DATA STRUCTURES
    cl_platform_id *platform;
    cl_uint n_platforms,NDEV,pindx;
    cl_device_id *device;
    cl_int err;
    //FIRST CALL IS FOR ASKING THE NUMBER OF PLATFORMS AVAILABLE:
    err = clGetPlatformIDs(0,NULL,&n_platforms);
    check_error(err,"ASKING FOR NUM OF PLATFORMS");
    //MEMORY ALLOCATION FOR THE PLATFOFORMS
    platform = (cl_platform_id *)malloc(sizeof(cl_platform_id)*n_platforms);
    printf("************************************************************************\n");
    printf("NUMBER OF OPENCL PLATFORMS: %d\n",n_platforms);

    //DATA STRUCTURE CREATION FORM THE PLATFORMS
    err = clGetPlatformIDs(n_platforms,platform,NULL);
    check_error(err,"ALLOCATING THE PLATFORMS");

    for(cl_uint i=0; i<n_platforms; i++)
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
    printf("SELECT A PLATFORM:\n");
    scanf("%d",&pindx);
    printf("YOU SELECTED:\n");
    DisplayPlatformInfo(platform[pindx],CL_PLATFORM_VENDOR,"CL_PLATFORM_VENDOR");


    free(platform);
    free(device);
}
