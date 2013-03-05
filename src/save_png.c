#include <stdio.h>
#include <stdlib.h>
#include "png.h"
#include "config.h"

int save_png(char *filename, unsigned char *data, int width, int height)
{
    png_structp png_ptr;
    png_infop info_ptr;
    int i;
    FILE *file = fopen(filename, "wb");

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
       return 1;

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
       png_destroy_write_struct(&png_ptr,
         (png_infopp)NULL);
       return 2;
    }

    png_init_io(png_ptr, file);

    if (setjmp(png_jmpbuf(png_ptr)))
    {
       png_destroy_write_struct(&png_ptr, &info_ptr);
       fclose(file);
       return 3;
    }

    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr)))
        return 4;

    for (i = 0; i < height; i ++)
        png_write_row(png_ptr, &(data[i * width]));

    if (setjmp(png_jmpbuf(png_ptr)))
        return 5;

    png_write_end(png_ptr, NULL);

    fclose(file);

    return 0;
}

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
