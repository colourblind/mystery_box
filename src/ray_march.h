#ifndef MYSTERYBOX_RAY_MARCH_H
#define MYSTERYBOX_RAY_MARCH_H

#include "config.h"

void go(config c, float (*objectFunc)(config c, vec3));
float inside(config c, vec3 z);

void go_cl(config c);

#endif // MYSTERYBOX_RAY_MARCH_H