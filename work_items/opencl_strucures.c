#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef __APPLE__
#define CL_SILENCE_DEPRECATION
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

struct OpenCL_Stuff
{
	cl_platform_id *platforms;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel kernel;

};
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
void PrintPlatformInfo(cl_platform_id *platforms, cl_uint nplat)
{
    for(cl_uint i=0; i<nplat; i++)
    {
        DisplayPlatformInfo(platforms[i],CL_PLATFORM_VENDOR,"CL_PLATFORM_VENDOR");
       // device = GetDeviceInPlatform(platform[i],CL_DEVICE_TYPE_ALL,&NDEV);
        printf("NUMBER OF DEVICES PER PLATFORM: %d\n",nplat);
    }
}
void GetPlatform(struct OpenCL_Stuff *opencl)
{
	cl_int err;
	cl_uint nplat;

	err = clGetPlatformIDs(0,NULL,&nplat);
	CheckError(err,"CREATING THE PLATFORM");
	
	opencl->platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id)*nplat);

	err = clGetPlatformIDs(nplat,opencl->platforms,NULL);
	CheckError(err,"ALLOCATING THE PLATFORM");

	PrintPlatformInfo(opencl->platforms,nplat);

}	
int main (void)
{
	struct OpenCL_Stuff opencl;


	GetPlatform(&opencl);
}
