#include "cl_utils.h"
#include "save_png.h"
#include "config.h"

void go_cl(config c)
{
    cl_int error;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue commands;
    cl_program program;
    cl_kernel kernel;
    cl_event ev;
    cl_mem mem;
    float *result = NULL;
    char buf[256];
    int work_size[2] = { c.width, c.height };

    cl_float4 camera_pos = { c.camera_pos.x, c.camera_pos.y, c.camera_pos.z, 0 };
    cl_float4 camera_dir = { 
        c.camera_target.x - c.camera_pos.x, 
        c.camera_target.y - c.camera_pos.y, 
        c.camera_target.z - c.camera_pos.z, 
        0 };
    cl_float4 light_pos = { c.light_pos.x, c.light_pos.y, c.light_pos.z, 0 };

    sprintf(buf, "-DBAILOUT=%d -DSCALE=%f -DFOV=%f", c.bailout, c.scale, c.fov);

    printf("Starting\n");

    init_cl(&platform, &device, &context, &commands);

    //dump_info(platform, device);

    printf("Ready\n");

    program = load_program_from_file("kernel.cl", context, device, buf);
    kernel = create_kernel(program, "test");

    mem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * c.width * c.height, NULL, &error);
    check_error(error, "Could not allocate buffer");
    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mem);
    error = clSetKernelArg(kernel, 1, sizeof(cl_float4), &camera_pos);
    error = clSetKernelArg(kernel, 2, sizeof(cl_float4), &camera_dir);
    error = clSetKernelArg(kernel, 3, sizeof(cl_float4), &light_pos);

    clFinish(commands);

    error = clEnqueueNDRangeKernel(commands, kernel, 2, NULL, work_size, NULL, 0, NULL, &ev);

    clFinish(commands);

    printf("Done\n");

    result = malloc(sizeof(float) * c.width * c.height);
    error = clEnqueueReadBuffer(commands, mem, CL_TRUE, 0, sizeof(float) * c.width * c.height, result, 0, NULL, &ev);

    save(result, c, 0, 0);

    free(result);
    clReleaseMemObject(mem);
    release_cl(context, program, commands, kernel);
}
