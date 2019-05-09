#include <stdio.h>
#include <stdlib.h>
#include "OPENCL_HEADERS.h"



void CheckError(cl_int err, char *str)
{
    if(err<0)
    {
        printf("SORRY,THERE IS AN ERROR IN %s\n",str);
        exit(1);
    }
}
