#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "vector.h"
#include "config.h"

#define DEGS_TO_RADS(t)		((t) / 180.0f * 3.141592654f)
#define CLAMP(t, mint, maxt)	(t < mint ? mint : (t > maxt ? maxt : t))

int save_png(char *filename, unsigned char *data, int width, int height);

void save(float **data, config c, float min_depth, float max_depth)
{
	int i, j;
	unsigned char *d = malloc(c.width * c.height * sizeof(unsigned char));

    memset(d, 0, c.width * c.height * sizeof(unsigned char));
	for (i = 0; i < c.width; i ++)
	{
		for (j = 0; j < c.height; j ++)
		{
			if (data[i][j] >= 0)
			{
                d[j * c.width + i] = (unsigned char)(data[i][j] * 255);
			}
		}
	}

	save_png(c.output_file, d, c.width, c.height);

	free(d);
}

float march(config c, vec3 start, vec3 dir, float (*objectFunc)(config, vec3))
{
    const float MIN_DISTANCE = 0.00025f;
    const float MAX_DISTANCE = 30; // TODO: dynamic, based on camera position
    float march = 0, distance;
    vec3 pos;

    while(1)
    {
        pos = vec_add(start, vec_mult(dir, march));

        distance = objectFunc(c, pos);
        if (distance < MIN_DISTANCE)
            return march;
        if (march > MAX_DISTANCE)
            break;
    
        march += distance;
    }

    return -1;
}

float colour(config c, vec3 position, float (*objectFunc)(config, vec3))
{
    const float ambient_scale = 0.1f;
    vec3 light_dir = vec_norm(vec_sub(c.light_pos, position));
    vec3 ao_sample_pos;
    float diffuse = 0, ambient;

    float x0 = objectFunc(c, vec_add_c(position, -c.normal_diff.x, 0, 0));
    float x1 = objectFunc(c, vec_add_c(position, c.normal_diff.x, 0, 0));
    float y0 = objectFunc(c, vec_add_c(position, 0, -c.normal_diff.y, 0));
    float y1 = objectFunc(c, vec_add_c(position, 0, c.normal_diff.y, 0));
    float z0 = objectFunc(c, vec_add_c(position, 0, 0, -c.normal_diff.z));
    float z1 = objectFunc(c, vec_add_c(position, 0, 0, c.normal_diff.z));
    
    vec3 normal = { x1 - x0, y1 - y0, z1 - z0 };

    normal = vec_norm(normal);

    // March a ray back towards the light, and do diffuse calculation
    // if we get there. We shim the shadow ray slightly so it doesn't
    // get 'caught' in the volume
    if (march(c, vec_add(position, vec_mult(light_dir, 0.01f)), vec_mult(light_dir, 1), objectFunc) < 0)
        diffuse = CLAMP(vec_dot(normal, light_dir), 0, 1);

    // For ambient occlusion, we back up slightly and check the DE
    // result from there.
    ao_sample_pos = vec_add(position, vec_mult(normal, 0.025f));
    ambient = objectFunc(c, ao_sample_pos);
    ambient = CLAMP(ambient * 100, 0, 1);

    return diffuse * (1 - ambient_scale) + ambient * ambient_scale;
}

void go(config c, float (*objectFunc)(config c, vec3))
{
    int x, y;
    float **depth;
    float half_fov_h = DEGS_TO_RADS(c.fov / 2);
    float half_fov_v = DEGS_TO_RADS((c.fov / 2) * ((float)c.height / c.width));
    float half_width = (float)c.width / 2;
    float half_height = (float)c.height / 2;
    vec3 camera_dir, ray_dir;
	float min_depth = 100000000;
	float max_depth = 0;
    float result;
	time_t start, end;

    // Init depth array
    depth = (float **)malloc(c.width * sizeof(float));
    for (x = 0; x < c.width; x ++)
        depth[x] = (float *)malloc(c.height * sizeof(float));

    camera_dir = vec_norm(vec_sub(c.camera_target, c.camera_pos));

	time(&start);
        
    // Iterate over pixels
    for (x = 0; x < c.width; x ++)
    {
        for (y = 0; y < c.height; y ++)
        {
            depth[x][y] = -1;
            // Generate dir vector
            ray_dir.x = tanf(((x - half_width) / half_width) * half_fov_h);
            ray_dir.y = tanf(((y - half_height) / half_height) * half_fov_v);
            ray_dir.z = 1;
            ray_dir = vec_norm(vec_rotate(ray_dir, camera_dir));
            // Ten hut!
            result = march(c, c.camera_pos, ray_dir, objectFunc);
            if (result > 0)
            {
                vec3 pos = vec_add(c.camera_pos, vec_mult(ray_dir, result));
                min_depth = result < min_depth ? result : min_depth;
                max_depth = result > max_depth ? result : max_depth;
                c.normal_diff.x = tanf(((x + 0.5f - half_width) / half_width) * half_fov_h);
                c.normal_diff.y = tanf(((y + 0.5f - half_height) / half_height) * half_fov_v);
                c.normal_diff.z = 1;
                c.normal_diff = vec_norm(vec_rotate(c.normal_diff, camera_dir));
                c.normal_diff = vec_sub(vec_mult(c.normal_diff, result), vec_mult(ray_dir, result));
                c.normal_diff.z = c.normal_diff.x;
                //c.normal_diff.x = 0.025f;
                //c.normal_diff.y = 0.025f;
                //c.normal_diff.z = 0.025f;
                c.normal_diff.x = absf(c.normal_diff.x);
                c.normal_diff.y = absf(c.normal_diff.y);
                c.normal_diff.z = absf(c.normal_diff.z);
                depth[x][y] = colour(c, pos, objectFunc);
            }
        }
		printf(".");
        fflush(stdout);
    }

	time(&end);
	printf("\nTime taken: %.2lf\n", difftime(end, start));

	save(depth, c, min_depth, max_depth);

    // Tear down array
    for (x = 0; x < c.width; x++)
        free(depth[x]);
    free(depth);
}

vec3 iterate(config c, vec3 v, vec3 z, float *dz)
{
    float m;
    float rmin = 0.5f;
    float rfix = 1;
    

    // Box fold
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
        
    // Sphere fold
    m = vec_length(v);
    if (m < 0.5)
    {
        v = vec_mult(v, 4);
        (*dz) *= 4;
    }
    else if (m < 1)
    {
        v = vec_mult(v, (1.f / (m * m)));
        (*dz) *= 1.f / (m * m);
    }
    
    v = vec_mult(v, c.scale);
    v = vec_add(v, z);
    
    (*dz) = (*dz) * absf(c.scale) + 1.f;
            
    return v;
}

float inside(config c, vec3 z)
{
    float lolwut = powf(2.f, 1 - c.bailout);
    int i;
    vec3 v;
    float dr = 1.f;
    
    v = z;
    for (i = 0; i < c.bailout; i ++)
        v = iterate(c, v, z, &dr);
    
    return vec_length(v) / absf(dr) - lolwut;
}

int main(int argc, char **argv)
{
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

	go(c, &inside);
}
