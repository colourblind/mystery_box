#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const float march_limit = 50; // Distance before we give up
const float march_step_broad = 0.05f; // Depth increment during broad phase testing
const float march_step = 0.0000005f; // Depth increment in each iteration
const float fov = 90; // Horizontal field of view

#define DEGS_TO_RADS(t)		((t) / 180.0f * 3.141592654f)
#define CLAMP(t, mint, maxt)	((t) < (mint) ? (mint) : ((t) > (maxt) ? (maxt) : (t)))
#define ABSF(f)		((f) < 0 ? (f) * -1 : (f))
#define MIN(x, y)   ((x) < (y) ? (x) : (y))
#define MAX(x, y)   ((x) > (y) ? (x) : (y))

int save_png(char *filename, unsigned char *data, int width, int height);

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

void save(float **data, int width, int height, float min_depth, float max_depth)
{
	int i, j;
	unsigned char *d = malloc(width * height * sizeof(unsigned char));

	memset(d, 0, width * height * sizeof(unsigned char));
	for (i = 0; i < width; i ++)
	{
		for (j = 0; j < height; j ++)
		{
			if (data[i][j] >= 0)
			{
				float fog = (CLAMP(data[i][j], min_depth, max_depth) - min_depth) / (max_depth - min_depth);
				d[j * width + i] = (unsigned char)((1 - fog) * 255);
			}
		}
	}

	save_png("out.png", d, width, height);

	free(d);
}

void go(int width, int height, int (*objectFunc)(vec3))
{
    int i;
    int x, y;
    float **depth;
    float march;
    float half_fov_h = DEGS_TO_RADS(fov / 2);
    float half_fov_v = DEGS_TO_RADS((fov / 2) * ((float)height / width));
    float half_width = (float)width / 2;
    float half_height = (float)height / 2;
    vec3 camera_pos, pos, dir;
	float min_depth = march_limit, max_depth = 0;
    int test_result;

    // Init depth array
    depth = (float **)malloc(width * sizeof(float));
    for (i = 0; i < width; i ++)
        depth[i] = (float *)malloc(height * sizeof(float));

	camera_pos.x = 0;
	camera_pos.y = 0;
	camera_pos.z = -10;
        
    // Iterate over pixels
    for (x = 0; x < width; x ++)
    {
        for (y = 0; y < height; y ++)
        {
            depth[x][y] = -1;
            // Generate dir vector
            dir.x = sinf(((x - half_width) / width) * half_fov_h);
            dir.y = sinf(((y - half_height) / height) * half_fov_v);
            dir.z = cosf(((x - half_width) / width) * half_fov_h);
            dir = vec_norm(dir); // necessary?
            // Ten hut!
            march = 0;
            while (march < march_limit)
            {
                pos = vec_add(camera_pos, vec_mult(dir, march));
                test_result = objectFunc(pos);

                if (objectFunc(pos) > 0)
                {
                    depth[x][y] = march;
                    min_depth = MIN(min_depth, march);
                    max_depth = MAX(max_depth, march);
                    break;
                }
                else if (test_result == 0) // small steps
                    march += march_step;
                else // miles out. Large steps
                    march += march_step_broad;
            }
        }
        printf(".");
    }

	save(depth, width, height, min_depth, max_depth);

    // Tear down array
    for (i = 0; i < width; i++)
        free(depth[i]);
    free(depth);
}

float test_object(vec3 v)
{
    if (v.x > 1 && v.x < 3 && v.y > 1 && v.y < 3 && v.z > -1 && v.z < 1)
        return 1;
    else
        return -1;
}

vec3 fold(vec3 v)
{
    float r = 0.5f;
    float f = 1;
	float mag;

    // Box folds for each component
	if (v.x > 1)
		v.x = 2 - v.x;
	else if (v.x < -1)
		v.x = -2 - v.x;

	if (v.y > 1)
		v.y = 2 - v.y;
	else if (v.y < -1)
		v.y = -2 - v.y;

	if (v.z > 1)
		v.z = 2 - v.z;
	else if (v.z < -1)
		v.z = -2 - v.z;

    v = vec_mult(v, f);

    // Sphere fold
	mag = vec_length(v);
	if (mag < r)
		v = vec_mult(v, 1.f / (r * r));
	else if (mag < 1) 
		v = vec_mult(v, 1.f / mag);

	return v;
}

int mandelbox(vec3 c)
{
    int attempts;
    int attempt_limit = 10000;
    float scale = 2;
    float bounds = 6;
    vec3 p;

    if (ABSF(c.x) > bounds + march_step_broad || ABSF(c.y) > + march_step_broad || ABSF(c.z) > + march_step_broad)
        return -1;

	p.x = p.y = p.z = 0;
	for (attempts = 0; attempts < attempt_limit; attempts ++)
	{
		p = vec_add(vec_mult(fold(p), scale), c);
		if (ABSF(p.x) > bounds || ABSF(p.y) > bounds || ABSF(p.z) > bounds)
			break;
	}

    if (attempts == attempt_limit) // Escaped
        return 0;
    else
        return 1;
}

int main(int argc, char **argv)
{
    go(120, 90, &mandelbox);
}
