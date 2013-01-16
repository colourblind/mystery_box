#include <stdlib.h>
#include <math.h>
#include <time.h>

#define DEGS_TO_RADS(t)		((t) / 180.0f * 3.141592654f)
#define CLAMP(t, mint, maxt)	(t < mint ? mint : (t > maxt ? maxt : t))

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
	float f;
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

float march(vec3 start, vec3 dir, int(*objectFunc)(vec3))
{
	const float march_limit = 25; // Distance before we give up
	const float march_step = 0.0005f; // Depth increment in each iteration
    float march = 10;
    vec3 pos;

    while (march < march_limit)
    {
        pos = vec_add(start, vec_mult(dir, march));
        if (objectFunc(pos) >= 0)
            return march;
    
        march += march_step;
    }

    return -1;
}

void go(int width, int height, int (*objectFunc)(vec3))
{
	const float fov = 90; // Horizontal field of view
    int x, y;
    float **depth;
    float half_fov_h = DEGS_TO_RADS(fov / 2);
    float half_fov_v = DEGS_TO_RADS((fov / 2) * ((float)height / width));
    float half_width = (float)width / 2;
    float half_height = (float)height / 2;
    vec3 camera_pos, dir;
	float min_depth = 100000000;
	float max_depth = 0;
    float result;
	time_t start, end;

    // Init depth array
    depth = (float **)malloc(width * sizeof(float));
    for (x = 0; x < width; x ++)
        depth[x] = (float *)malloc(height * sizeof(float));

	camera_pos.x = 0;
	camera_pos.y = 0;
	camera_pos.z = -17;

	time(&start);
        
    // Iterate over pixels
    for (x = 0; x < width; x ++)
    {
        for (y = 0; y < height; y ++)
        {
            depth[x][y] = -1;
            // Generate dir vector
            dir.x = tanf(((x - half_width) / half_width) * half_fov_h);
            dir.y = tanf(((y - half_height) / half_height) * half_fov_v);
            dir.z = 1;
            dir = vec_norm(dir); // necessary?
            // Ten hut!
            result = march(camera_pos, dir, objectFunc);
            if (result > 0)
            {
			    min_depth = result < min_depth ? result : min_depth;
			    max_depth = result > max_depth ? result : max_depth;
                depth[x][y] = result;
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

int test_object(vec3 v)
{
    if (v.x > -6 && v.x < 6 && v.y > -6 && v.y < 6 && v.z > -6 && v.z < 6)
        return 1;
    else
        return -1;
}

vec3 iterate(vec3 v, vec3 c)
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
        v = vec_mult(v, 4);
    else if (m < 1)
        v = vec_mult(v, (1.f / (m * m)));
    
    v = vec_mult(v, scale);
    v = vec_add(v, c);
    
    return v;
}

int inside(vec3 c, float *march)
{
    int bailout_limit = 15;
    int i;
    vec3 v;
    
    v = c;
    for (i = 0; i < bailout_limit; i ++)
    {
        v = iterate(v, c);
        
		if (vec_length(v) > 20)
            return -1;
    }
    
    return 1;
}


int main(int argc, char **argv)
{
    int width = argc < 2 ? 480 : atoi(argv[1]);
    int height = argc < 3 ? 360 : atoi(argv[2]);

	go(width, height, &inside);
}
