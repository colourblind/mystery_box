#include "cl_utils.h"

void check_error(const cl_int error, const char *name)
{
    if (error != CL_SUCCESS)
    {
        fprintf(stderr, "ERROR: %s (%d)\n", name, error);
        exit(1);
    }
}

void dump_info(const cl_platform_id platform, const cl_device_id device)
{
    char buf[256];
    cl_uint integer;
    cl_ulong longint;

    clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(buf), buf, NULL);
    printf("CL_PLATFORM_VENDOR            : %s\n", buf);
    clGetPlatformInfo(platform, CL_PLATFORM_NAME, sizeof(buf), buf, NULL);
    printf("CL_PLATFORM_NAME              : %s\n", buf);
    clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(buf), buf, NULL);
    printf("CL_PLATFORM_VERSION           : %s\n", buf);
    clGetPlatformInfo(platform, CL_PLATFORM_PROFILE, sizeof(buf), buf, NULL);
    printf("CL_PLATFORM_PROFILE           : %s\n", buf);

    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(buf), buf, NULL);
    printf("CL_DEVICE_NAME                : %s\n", buf);
    clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(buf), buf, NULL);
    printf("CL_DEVICE_VERSION             : %s\n", buf);
    clGetDeviceInfo(device, CL_DRIVER_VERSION, sizeof(buf), buf, NULL);
    printf("CL_DRIVER_VERSION             : %s\n", buf);

    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(longint), &longint, NULL);
    printf("CL_DEVICE_GLOBAL_MEM_SIZE     : %lu\n", longint);
    clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(longint), &longint, NULL);
    printf("CL_DEVICE_LOCAL_MEM_SIZE      : %lu\n", longint);
    clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(integer), &integer, NULL);
    printf("CL_DEVICE_MAX_CLOCK_FREQUENCY : %u\n", integer);
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(integer), &integer, NULL);
    printf("CL_DEVICE_MAX_COMPUTE_UNITS   : %u\n", integer);
}

cl_platform_id get_platform()
{
    cl_platform_id platform;
    check_error(clGetPlatformIDs(1, &platform, NULL), "get_platform");
    return platform;
}

cl_device_id get_device(const cl_platform_id platform)
{
    cl_device_id device;
    check_error(clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL), "get_device");
    return device;
}

cl_context get_context(const cl_device_id device)
{
    cl_int error;
    cl_context context = clCreateContext(0, 1, &device, NULL, NULL, &error);
    check_error(error, "get_context");
    return context;
}

cl_command_queue create_command_queue(const cl_context context, const cl_device_id device)
{
    cl_int error;
    cl_command_queue commands = clCreateCommandQueue(context, device, 0, &error);
    check_error(error, "create_command_queue");
    return commands;
}

void init_cl(cl_platform_id *platform, cl_device_id *device, cl_context *context, cl_command_queue *commands)
{
    *platform = get_platform();
    *device = get_device(*platform);
    *context = get_context(*device);
    *commands = create_command_queue(*context, *device);
}

cl_program load_program_from_file(const char *filename, const cl_context context, const cl_device_id device, const char *params)
{
    char *source = NULL;
    size_t file_size;
    cl_program program;
    FILE *f = fopen(filename, "rb");

    if (!f)
    {
        fprintf(stderr, "ERROR: Could not find source file '%s'\n", filename);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    file_size = ftell(f) + 1;
    fseek(f, 0, SEEK_SET);

    source = calloc(file_size, sizeof(char));
    fread(source, sizeof(char), file_size, f);

    fclose(f);

    program = load_program_from_source(source, context, device, params);

    free(source);

    return program;
}

cl_program load_program_from_source(const char *source, const cl_context context, const cl_device_id device, const char *params)
{
    cl_int error;
    cl_program program;
    program = clCreateProgramWithSource(context, 1, &source, NULL, &error);
    check_error(error, "load kernel");
    error = clBuildProgram(program, 0, NULL, params, NULL, NULL);
    if (error != CL_SUCCESS)
    {
        char build_log[40000];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, NULL);
        check_error(error, build_log);
    }
    return program;
}

cl_kernel create_kernel(const cl_program program, const char *entry_point)
{
    cl_int error;
    cl_kernel kernel = clCreateKernel(program, entry_point, &error);
    check_error(error, "create_kernel");
    return kernel;
}

void release_cl(cl_context context, cl_program program, cl_command_queue commands, cl_kernel kernel)
{
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);
}

