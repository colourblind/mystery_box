#include <stdio.h>
#include <stdlib.h>
#include "opencl.h"

void check_error(const cl_int error, const char *name);

void init_cl(cl_platform_id *platform, cl_device_id *device, cl_context *context, cl_command_queue *commands);
cl_program load_program_from_file(const char *filename, const cl_context context, const cl_device_id device, const char *params);
cl_program load_program_from_source(const char *source, const cl_context context, const cl_device_id device, const char *params);
cl_kernel create_kernel(const cl_program program, const char *entry_point);

void dump_info(const cl_platform_id platform, const cl_device_id device);

void release_cl(cl_context context, cl_program program, cl_command_queue commands, cl_kernel kernel);
