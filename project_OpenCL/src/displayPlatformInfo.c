#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "OPENCL_HEADERS.h"

extern void CheckError(cl_int err, char *str);
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
