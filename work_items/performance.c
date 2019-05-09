//
//  performance.c
//  
//
//  Created by Juan García on 2/26/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PROGRAM_FILE "new_work_item_ker.cl"
#define KERNEL_NAME_FUNC "new_kernel"

#ifdef __APPLE__
#define CL_SILENCE_DEPRECATION
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif


void CheckError(cl_int err, char *str)
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
    CheckError(err,"GETTING THE PLATFORM INFO");
    printf("PLATFORM VENDOR NAME: %s\n",out_info);
    
    free(out_info);
}
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
                case CL_DEVICE_GLOBAL_MEM_SIZE: printf("\tDEVICE GLOBAL MEM: %llu MB\n",(*size)>>20);break;
                case CL_DEVICE_MAX_MEM_ALLOC_SIZE: printf("\tDEVICE MAX MEMORY ALLOCATION %llu MB\n",(*size)>>20);break;
            }
            free(size);
        }break;
    }// END OF THE MASTER SWITCH
    
    
}
void GetOpenCLInfo()
{
    //FUNCTION TO GET THE OPENCL INFORMATION OF THE COMPUTER SYSTEM
    
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

void GetComputeUnits(cl_device_id did)
{
    cl_int err;
    cl_uint cu,wi,max_WIDs_dim;
    CheckError(err,"GETTING COMPUTE UNITS");
    err = clGetDeviceInfo(did,CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(cu),&cu,NULL);
    CheckError(err,"GETTING COMPUTE UNITS");
    err = clGetDeviceInfo(did,CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,sizeof(cl_uint),&max_WIDs_dim,NULL);
    size_t *arr = (size_t *)malloc(sizeof(size_t)*CL_DEVICE_MAX_WORK_ITEM_SIZES);
    
    err = clGetDeviceInfo(did,CL_DEVICE_MAX_WORK_ITEM_SIZES,CL_DEVICE_MAX_WORK_ITEM_SIZES,arr,NULL);
    
    printf("NUMBER OF MAX PARALLEL COMPUTE UNITS: %d\n",cu);

    printf("DIMENSIONS: %d\n",max_WIDs_dim);
    for(cl_uint i=0; i<max_WIDs_dim; i++)
    {
        printf("%zu  ",arr[i]);
    }
    printf("\n");
    

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

void user_input(int *Nx,int *Ny, int *Nz,float *x0, float  *xN,float *y0,float *yN,float *z0,float *zN,float *hx, float *hy,float *hz,int *Ntot)
{
    //MODULE TO GET THE USER INPUT
    printf("NUMBER OF POINTS IN X,Y & Z\n");
    scanf("%d %d %d",Nx,Ny,Nz);
    printf("INITIAL AN FINAL POINTS IN X COORDINATE\n");
    scanf("%f %f",x0,xN);
    printf("INITIAL AN FINAL POINTS IN Y COORDINATE\n");
    scanf("%f %f",y0,yN);
    printf("INITIAL AN FINAL POINTS IN Z COORDINATE\n");
    scanf("%f %f",z0,zN);
    
    
    
    *hx = (*xN-(*x0))/(float)(*Nx-1);
    *hy = (*yN-(*y0))/(float)(*Ny-1);
    *hz = (*zN-(*z0))/(float)(*Nz-1);
    
    *Ntot = (*Nx)*(*Ny)*(*Nz);
    
    printf("STEPS IN X = %f, STEPS IN Y = %f, STEPS IN Z = %f\n",*hx,*hy,*hz);
    printf("TOTAL POINTS = %d\n",*Ntot);
}

int main ()
{
    
    
    cl_uint pIdx,dIdx;
    
    // **+ VARIABLES FOR THE USER DATA ***
    int Nx,Ny,Nz,Ntot;
    float x0,xN,y0,yN,z0,zN,hx,hy,hz;
    
    //CALL TO THE FUNCTION THAT ASKS FOR THE USER DATA
    user_input(&Nx,&Ny,&Nz,&x0,&xN,&y0,&yN,&z0,&zN,&hx,&hy,&hz,&Ntot);
    //******************************************************************
    
    float step[] = {hx,hy,hz};
    float point0[] ={x0,y0,z0};
    
    
    //THE FIRST STEP IN OPENCL IS QUERY FOR THE AVAILABLE HARDWARE
    
    GetOpenCLInfo();
    
    //THE ABOVE FUNCTION DISPLAYS THE AVAILABLE RESOURCES AND YOU MUST SELECT THE PLATFORM AND THE DEVICE TO PERFORM YOUR COMPUTATIONS ********
    printf("SELECT THE ID FOR THE PLATFORM\n");
    scanf("%d",&pIdx);
    printf("SELECT THE ID FOR THE DEVICE\n");
    scanf("%d",&dIdx);
    
    printf("THE OPENCL CONTEXT WILL BE CREATED WITH THE FOLLOWING DATA:\n");

    // MEMORY OBJECTS *********
    cl_mem read_buff,buff_step,buff_points;
    //PLATFORM, DEVICE AND CONTEXT DATA STRUCTURES******
    cl_platform_id *platform;
    cl_device_id *device;
    cl_context context;
    cl_int err;
    
    //MEMORY ALLOCATION FOR THE PLATFORM AND DEVICE DATA STRUCTURES *****
    platform = (cl_platform_id *)malloc(sizeof(cl_device_id)*(pIdx+1));
    device = (cl_device_id*)malloc(sizeof(cl_device_id)*(dIdx+1));
    // ******************************************************************
    // STORE THE PLATFORM AND DEVICE DATA STRUCTURES*********************
    err = clGetPlatformIDs(pIdx+1,platform,NULL);
    CheckError(err,"GETTING YOUR SELECTED PLARTFORM\n");
    err = clGetDeviceIDs(platform[pIdx],CL_DEVICE_TYPE_ALL,dIdx+1,device,NULL);
    CheckError(err,"GETTING YOUR SELECTED DEVICES\n");
    DisplayPlatformInfo(platform[pIdx],CL_PLATFORM_VENDOR,"CL_PLATFORM_VENDOR");
    DisplayDeviceInfo(device[dIdx],CL_DEVICE_TYPE);
    DisplayDeviceInfo(device[dIdx],CL_DEVICE_VENDOR);
    DisplayDeviceInfo(device[dIdx],CL_DEVICE_NAME);
    //THIS FUNCTION SHOWS THE COMPUTE UNITS FOR THE SELECTED DEVICE *************
    GetComputeUnits(device[dIdx]);
    // ******************************************************************
    // OPENCL CONTEXT CREATION WITH THE PLATFORM AND DEVICES SELECTED BY THE USER ****
    context = clCreateContext(NULL,1,&device[dIdx],NULL,NULL,&err);
    CheckError(err,"CREATING THE CONTEXT");
    //********************************************************************
    
    // PROGRAM AND KERNEL CREATION ***************************************
    program = BuildProgram(context,device,PROGRAM_FILE);
    
    kernel = clCreateKernel(program,KERNEL_NAME_FUNC,&err);
    CheckError(err,"KERNEL CREATION");
    //WORK-ITEMS DATA *************************
    size_t dim = 2;
    size_t global_offset[] = {0,0};
    size_t global_size[] = {Gx,Gy};
    size_t local_size[]  = {Lx,Ly};
    

    
    
    
    
    free(device);
    free(platform);
    clReleaseContext(context);
    
    
    return 0;
    
}
