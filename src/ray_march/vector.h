typedef struct vec3
{
    float x, y, z;
} vec3;

vec3 vec_add(vec3 a, vec3 b)
{
    vec3 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
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
	return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
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

float absf(float a)
{
    return a < 0 ? -a : a;
}
