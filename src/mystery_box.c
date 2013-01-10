#include <stdio.h>
#include <time.h>
#include <math.h>

const float scale = 2.f;
const int attempt_limit = 10000;
const float bounds = 6.f;

#define ABSF(f)		(f < 0 ? f * -1 : f)

float magnitude(float *v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float *vec_add(float *v, float *w)
{
	v[0] += w[0];
	v[1] += w[1];
	v[2] += w[2];
	return v;
}

float *vec_scale(float *v, float s)
{
	v[0] = v[0] * s;
	v[1] = v[1] * s;
	v[2] = v[2] * s;
	return v;
}

float *iterate(float *v)
{
	int i = 0;
	float mag;

	for (i = 0; i < 3; i ++)
	{
		if (v[i] > 1)
			v[i] = 2 - v[i];
		else if (v[i] < -1)
			v[i] = -2 - v[i];
	}

	mag = magnitude(v);
	if (mag < 0.5f)
		vec_scale(v, 1.f / (0.5 * 0.5));
	else if (mag < 1) 
		vec_scale(v, 1.f / (mag * mag));

	return v;
}

int main(int argc, char **argv)
{
	int attempts;
	float v[3];
	float c[3];
	int last_percentage = 0;
	int current_percentage;
	time_t start, end;
	int samples_per_side = argc > 1 ? atoi(argv[1]) : 49;
	float step = (bounds * 2) / (samples_per_side - 1);
    int i, j, k;

	FILE *f = fopen("out.txt", "w");
    fprintf(f, "# samples (%d) bounds (%.2f) block_size (%.2f) escape_limit (%d)\n", samples_per_side, bounds, (bounds * 2) / samples_per_side, attempt_limit);
	time(&start);

	printf("Starting with %d samples per side\n", samples_per_side);

	for (i = 0; i <= samples_per_side; i ++)
	{
		for (j = 0; j <= samples_per_side; j ++)
		{
			for (k = 0; k <= samples_per_side; k ++)
			{
                c[0] = ((float)i / samples_per_side) * (bounds * 2) - bounds;
                c[1] = ((float)j / samples_per_side) * (bounds * 2) - bounds;
                c[2] = ((float)k / samples_per_side) * (bounds * 2) - bounds;
				v[0] = v[1] = v[2] = 0;
				for (attempts = 0; attempts < attempt_limit; attempts ++)
				{
					vec_add(vec_scale(iterate(v), scale), c);
					if (ABSF(v[0]) > bounds || ABSF(v[1]) > bounds || ABSF(v[2]) > bounds)
						break;
				}

				if (attempts == attempt_limit)
					fprintf(f, "%.4f\t%.4f\t%f\n", c[0], c[1], c[2]);
				//else
				//	fprintf(f, "%.4f\t%.4f\t%f\tESCAPED\n", c[0], c[1], c[2]);
			}
		}
		current_percentage = ((int)((c[0] + bounds) * 100 / (bounds * 2)));
        if (current_percentage >= last_percentage + 5)
		{
			last_percentage = current_percentage;
            printf("%d%%", last_percentage);
		}
		else
			printf(".");
		//break;
	}

	fclose(f);
	time(&end);
	printf("\nTime taken: %.2lf\n", difftime(end, start));

	return 0;
}
