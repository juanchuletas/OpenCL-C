#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "OPENCL_HEADERS.h"


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
                case CL_DEVICE_TYPE_CPU: printf("== CPU == \n");break;
                case CL_DEVICE_TYPE_GPU: printf("== GPU == \n");break;
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
                case CL_DEVICE_NAME: printf("\tDEVICE NAME: %s\n",data);break;
                case CL_DEVICE_VENDOR: printf("\tDEVICE_VENDOR: %s\n",data);break;
                    
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
                case CL_DEVICE_GLOBAL_MEM_SIZE: printf("\tDEVICE GLOBAL MEM: %lu MB\n",(*size)>>20);break;
                case CL_DEVICE_MAX_MEM_ALLOC_SIZE: printf("\tDEVICE MAX MEMORY ALLOCATION %lu MB\n",(*size)>>20);break;
            }
            free(size);
        }break;
    }// END OF THE MASTER SWITCH
    
    
}
