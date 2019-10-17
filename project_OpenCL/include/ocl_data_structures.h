#include "OPENCL_HEADERS.h"
struct OPENCL_STRUCTURES
{
    cl_platform_id *platform;
    cl_device_id *device;
    cl_context context;
    cl_command_queue cQ;
    cl_program *program;
    cl_kernel *kernel;
};
// ****************************************************
// STRUCTURES TO STORE THE OPENCL MEMORY OBJECTS ******
struct Node //STRUCTS DECLARATION
{
        int num;
        struct Node* next;
};

