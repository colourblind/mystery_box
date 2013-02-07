#ifndef MYSTERYBOX_VECTOR_H
#define MYSTERYBOX_VECTOR_H

typedef struct
{
    float x, y, z;
} vec3;

vec3 vec_add(vec3 a, vec3 b);
vec3 vec_add_c(vec3 a, float x, float y, float z);
vec3 vec_sub(vec3 a, vec3 b);
vec3 vec_mult(vec3 a, float b);
float vec_length(vec3 a);
vec3 vec_norm(vec3 a);
float vec_dot(vec3 a, vec3 b);
vec3 vec_cross(vec3 a, vec3 b);
vec3 vec_rotate(vec3 a, vec3 forward);

float absf(float a);

#endif // MYSTERYBOX_VECTOR_H
