#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const double march_limit = 40; // Distance before we give up
const double march_step = 0.0000001f; // Depth increment in each iteration
const double march_step_broad = 0.01f;
const double fov = 90; // Horizontal field of view

#define DEGS_TO_RADS(t)		((t) / 180.0 * 3.141592654)
#define CLAMP(t, mint, maxt)	((t) < (mint) ? (mint) : ((t) > (maxt) ? (maxt) : (t)))
#define ABS(f)		((f) < 0 ? (f) * -1 : (f))
#define MIN(x, y)   ((x) < (y) ? (x) : (y))
#define MAX(x, y)   ((x) > (y) ? (x) : (y))

int save_png(char *filename, unsigned char *data, int width, int height);

typedef struct vec3
{
    double x, y, z;
} vec3;

vec3 vec_add(vec3 a, vec3 b)
{
    vec3 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return r;
}

vec3 vec_mult(vec3 a, double b)
{
    vec3 r;
    r.x = a.x * b;
    r.y = a.y * b;
    r.z = a.z * b;
    return r;
}

double vec_length_sq(vec3 a)
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

double vec_length(vec3 a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

vec3 vec_norm(vec3 a)
{
    vec3 r;
    double l = vec_length(a);
    r.x = a.x / l;
    r.y = a.y / l;
    r.z = a.z / l;
    return r;
}

void save(double **data, int width, int height, double min_depth, double max_depth)
{
	int i, j;
	unsigned char *d = malloc(width * height * sizeof(unsigned char));
    double fog;

	memset(d, 0, width * height * sizeof(unsigned char));
	for (i = 0; i < width; i ++)
	{
		for (j = 0; j < height; j ++)
		{
			if (data[i][j] >= 0)
			{
				fog = (CLAMP(data[i][j], min_depth, max_depth) - min_depth) / (max_depth - min_depth);
				d[j * width + i] = (unsigned char)((1 - fog) * 255);
				//d[j * width + i] = 255;
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
    double **depth;
    double march;
    double half_fov_h = DEGS_TO_RADS(fov / 2);
    double half_fov_v = DEGS_TO_RADS((fov / 2) * ((double)height / width));
    double half_width = (double)width / 2;
    double half_height = (double)height / 2;
    vec3 camera_pos, pos, dir;
	double min_depth = march_limit, max_depth = 0;
    int test_result;

    // Init depth array
    depth = (double **)malloc(width * sizeof(double));
    for (i = 0; i < width; i ++)
        depth[i] = (double *)malloc(height * sizeof(double));

	camera_pos.x = 0;
	camera_pos.y = 0;
	camera_pos.z = -16;
        
    // Iterate over pixels
    for (x = 0; x < width; x ++)
    {
        for (y = 0; y < height; y ++)
        {
            depth[x][y] = -1;
            // Generate dir vector
            dir.x = sin(((x - half_width) / width) * half_fov_h);
            dir.y = sin(((y - half_height) / height) * half_fov_v);
            dir.z = cos(((x - half_width) / width) * half_fov_h);
            dir = vec_norm(dir); // necessary?
            // Ten hut!
            march = 8; // cheat!
            while (march < march_limit)
            {
                pos = vec_add(camera_pos, vec_mult(dir, march));
                test_result = objectFunc(pos);

                if (test_result > 0)
                {
                    depth[x][y] = march;
                    min_depth = MIN(min_depth, march);
                    max_depth = MAX(max_depth, march);
                    break;
                }
                else if (test_result < 0)
                    march += march_step_broad;
                else
                    march += march_step;
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

int test_object(vec3 v)
{
    if (v.x > -1 && v.x < 1 && v.y > -1 && v.y < 1 && v.z > -1 && v.z < 1)
        return 1;
    else
        return -1;
}

vec3 iterate(vec3 v, vec3 c)
{
    double scale = 2;
    double fixed_radius = (1 * 1);
    double min_radius = (0.5 * 0.5);
	double mag;

    // Box folds for each component
	if (v.x > 1)
		v.x = scale - v.x;
	else if (v.x < -1)
		v.x = -scale - v.x;

	if (v.y > 1)
		v.y = scale - v.y;
	else if (v.y < -1)
		v.y = -scale - v.y;

	if (v.z > 1)
		v.z = scale - v.z;
	else if (v.z < -1)
		v.z = -scale - v.z;

    // Sphere fold
	mag = vec_length(v);
	if (mag < min_radius)
		v = vec_mult(v, (fixed_radius * fixed_radius) / (min_radius * min_radius));
	else if (mag < fixed_radius) 
		v = vec_mult(v, (fixed_radius * fixed_radius) / (mag * mag));

    v = vec_mult(v, scale);
    v = vec_add(v, c);

	return v;
}

int mandelbox(vec3 c)
{
    int attempts;
    int attempt_limit = 1000;
    double bounds = 2;
    vec3 p;
    double spacing = bounds + march_step_broad + march_step;

    if (ABS(c.x) > spacing || ABS(c.y) > spacing || ABS(c.z) > spacing)
        return -1;

	p.x = c.x;
    p.y = c.y;
    p.z = c.z;
	for (attempts = 0; attempts < attempt_limit; attempts ++)
	{
		p = iterate(p, c);
		//if (vec_length_sq(p) > 121)
        if (ABS(p.x) > spacing || ABS(p.y) > spacing || ABS(p.z) > spacing)
			return 0;
	}

    return 1;
}

int main(int argc, char **argv)
{
    //go(120, 90, &test_object);
    go(240, 180, &mandelbox);
}
