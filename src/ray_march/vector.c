#include <math.h>
#include "vector.h"

vec3 vec_add(vec3 a, vec3 b)
{
    vec3 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return r;
}

vec3 vec_add_c(vec3 a, float x, float y, float z)
{
    vec3 r;
    r.x = a.x + x;
    r.y = a.y + y;
    r.z = a.z + z;
    return r;
}

vec3 vec_sub(vec3 a, vec3 b)
{
    vec3 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    return r;
}

vec3 vec_mult(vec3 a, float b)
{
    vec3 r;
    r.x = a.x * b;
    r.y = a.y * b;
    r.z = a.z * b;
    return r;
}

float vec_length(vec3 a)
{
    float x = a.x * a.x;
    float y = a.y * a.y;
    float z = a.z * a.z;
    float wtf = x + y + z;
	return sqrtf(wtf);
}

vec3 vec_norm(vec3 a)
{
    vec3 r;
    float l = vec_length(a);
    r.x = a.x / l;
    r.y = a.y / l;
    r.z = a.z / l;
    return r;
}

float vec_dot(vec3 a, vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 vec_cross(vec3 a, vec3 b)
{
    vec3 r;
    r.x = a.y * b.z - a.z * b.y;
    r.y = a.z * b.x - a.x * b.z;
    r.z = a.x * b.y - a.y * b.x;
    return r;
}

vec3 vec_rotate(vec3 a, vec3 forward)
{
    vec3 r;
    vec3 up, side;

    up.x = 0;
    up.y = 1;
    up.z = 0;

    forward = vec_norm(forward);
    side = vec_cross(up, forward);
    up = vec_cross(side, forward);

    side = vec_norm(side);
    up = vec_norm(up);

    r.x = side.x * a.x + up.x * a.y + forward.x * a.z;
    r.y = side.y * a.x + up.y * a.y + forward.y * a.z;
    r.z = side.z * a.x + up.z * a.y + forward.z * a.z;

    return r;
}

float absf(float a)
{
    return a < 0 ? -a : a;
}
