#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ray_march.h"

int main(int argc, char **argv)
{
    time_t start, end;

    config c;
    c.bailout = 15;
    c.camera_pos.x = 15;
    c.camera_pos.y = 3;
    c.camera_pos.z = 15;
    c.camera_target.x = 0;
    c.camera_target.y = 0;
    c.camera_target.z = 0;
    c.fov = 90;
    c.height = 480;
    c.scale = 2;
    c.width = 640;
    c.light_pos.x = 9;
    c.light_pos.y = 3;
    c.light_pos.z = 2;
    strcpy(c.output_file, "out.png");

    if (argc > 1)
        load_config(argv[1], &c);
    c.width = argc > 2 ? atoi(argv[2]) : c.width;
    c.height = argc > 3 ? atoi(argv[3]) : c.height;

    time(&start);
    
    #ifdef MYSTERY_BOX_CL
        go_cl(c);    
    #else
        go(c, &inside);
    #endif

    time(&end);
    printf("\nTime taken: %.2lf\n", difftime(end, start));
}
