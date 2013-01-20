#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "vector.h"

#define DEGS_TO_RADS(t)		((t) / 180.0f * 3.141592654f)
#define CLAMP(t, mint, maxt)	(t < mint ? mint : (t > maxt ? maxt : t))

int save_png(char *filename, unsigned char *data, int width, int height);

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
				//float fog = (CLAMP(data[i][j], min_depth, max_depth) - min_depth) / (max_depth - min_depth);
				//d[j * width + i] = (unsigned char)((1 - fog) * 255);
                d[j * width + i] = (unsigned char)(data[i][j] * 255);
			}
		}
	}

	save_png("out.png", d, width, height);

	free(d);
}

float march(vec3 start, vec3 dir, float (*objectFunc)(vec3))
{
    const float MIN_DISTANCE = 0.0005f;
    const float MAX_DISTANCE = 30; // TODO: dynamic, based on camera position
    int step;
    float march = 0, distance;
    vec3 pos;

    while(1)
    {
        pos = vec_add(start, vec_mult(dir, march));

        distance = objectFunc(pos);
        if (distance < MIN_DISTANCE)
            return march;
        if (march > MAX_DISTANCE)
            break;
    
        march += distance;
    }

    return -1;
}

float colour(vec3 position, float (*objectFunc)(vec3))
{
    const float TAP_OFFSET = 0.025f;
    const float ambient_scale = 0.1f;
    vec3 light_pos = { 9, 3, 2 };
    vec3 light_dir = vec_norm(vec_sub(light_pos, position));
    vec3 ao_sample_pos;
    float diffuse = 0, ambient;

    float x0 = objectFunc(vec_add_c(position, -TAP_OFFSET, 0, 0));
    float x1 = objectFunc(vec_add_c(position, TAP_OFFSET, 0, 0));
    float y0 = objectFunc(vec_add_c(position, 0, -TAP_OFFSET, 0));
    float y1 = objectFunc(vec_add_c(position, 0, TAP_OFFSET, 0));
    float z0 = objectFunc(vec_add_c(position, 0, 0, -TAP_OFFSET));
    float z1 = objectFunc(vec_add_c(position, 0, 0, TAP_OFFSET));
    
    vec3 normal = { x1 - x0, y1 - y0, z1 - z0 };

    normal = vec_norm(normal);

    // March a ray back towards the light, and do diffuse calculation
    // if we get there. We shim the shadow ray slightly so it doesn't
    // get 'caught' in the volume
    if (march(vec_add(position, vec_mult(light_dir, 0.01f)), vec_mult(light_dir, 1), objectFunc) < 0)
        diffuse = CLAMP(vec_dot(normal, light_dir), 0, 1);

    // For ambient occlusion, we back up slightly and check the DE
    // result from there.
    ao_sample_pos = vec_add(position, vec_mult(normal, 0.025f));
    ambient = objectFunc(ao_sample_pos);
    ambient = CLAMP(ambient * 150, 0, 1);

    return diffuse * (1 - ambient_scale) + ambient * ambient_scale;
}

void go(int width, int height, float (*objectFunc)(vec3))
{
	const float fov = 90; // Horizontal field of view
    int x, y;
    float **depth;
    float half_fov_h = DEGS_TO_RADS(fov / 2);
    float half_fov_v = DEGS_TO_RADS((fov / 2) * ((float)height / width));
    float half_width = (float)width / 2;
    float half_height = (float)height / 2;
    vec3 camera_pos, camera_dir, ray_dir;
	float min_depth = 100000000;
	float max_depth = 0;
    float result;
	time_t start, end;

    // Init depth array
    depth = (float **)malloc(width * sizeof(float));
    for (x = 0; x < width; x ++)
        depth[x] = (float *)malloc(height * sizeof(float));

	camera_pos.x = 15;
	camera_pos.y = 4;
	camera_pos.z = 15;

    // Where (0, 0, 0) is our target
    camera_dir.x = 0 - camera_pos.x;
    camera_dir.y = 0 - camera_pos.y;
    camera_dir.z = 0 - camera_pos.z;

    camera_dir = vec_norm(camera_dir);

	time(&start);
        
    // Iterate over pixels
    for (x = 0; x < width; x ++)
    {
        for (y = 0; y < height; y ++)
        {
            depth[x][y] = -1;
            // Generate dir vector
            ray_dir.x = tanf(((x - half_width) / half_width) * half_fov_h);
            ray_dir.y = tanf(((y - half_height) / half_height) * half_fov_v);
            ray_dir.z = 1;
            ray_dir = vec_norm(vec_rotate(ray_dir, camera_dir));
            // Ten hut!
            result = march(camera_pos, ray_dir, objectFunc);
            if (result > 0)
            {
			    min_depth = result < min_depth ? result : min_depth;
			    max_depth = result > max_depth ? result : max_depth;
                depth[x][y] = colour(vec_add(camera_pos, vec_mult(ray_dir, result)), objectFunc);
            }
        }
		printf(".");
    }

	time(&end);
	printf("\nTime taken: %.2lf\n", difftime(end, start));

	save(depth, width, height, min_depth, max_depth);

    // Tear down array
    for (x = 0; x < width; x++)
        free(depth[x]);
    free(depth);
}

vec3 iterate(vec3 v, vec3 c, float *dz)
{
    float m;
    float rmin = 0.5f;
    float rfix = 1;
	float scale = 2;
    

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
    
    v = vec_mult(v, scale);
    v = vec_add(v, c);
    
    (*dz) = (*dz) * absf(scale) + 1.f;
            
    return v;
}

float inside(vec3 c)
{
    const int bailout_limit = 15;
    const float bounding_volume_radius = 11;
    float lolwut = powf(2.f, 1 - bailout_limit);
    int i;
    vec3 v;
    float dr = 1.f;
    
    v = c;
    for (i = 0; i < bailout_limit; i ++)
        v = iterate(v, c, &dr);
    
    return vec_length(v) / absf(dr) - lolwut;
    //return (vec_length(v) - bounding_volume_radius) / absf(dr);
}

int main(int argc, char **argv)
{
    int width = argc < 2 ? 320 : atoi(argv[1]);
    int height = argc < 3 ? 240 : atoi(argv[2]);

	go(width, height, &inside);
}
