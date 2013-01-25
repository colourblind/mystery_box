#ifndef MYSTERYBOX_CONFIG_H
#define MYSTERYBOX_CONFIG_H

#include "vector.h"

typedef struct
{
    vec3 camera_pos;
    vec3 camera_target;
    float scale;
	int bailout;
	int width;
	int height;
    float fov;
    float normal_diff;
    vec3 light_pos;
} config;

int load_config(char *filename, config *c);

#endif // MYSTERYBOXMYSTERYBOX_CONFIG_H
