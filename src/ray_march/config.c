#include <stdio.h>
#include <string.h>
#include "config.h"

void handle(char *key, char *value, config *c)
{
	if (strcmp(key, "camera_pos") == 0)
		sscanf(value, "%f %f %f", &(c->camera_pos.x), &(c->camera_pos.y), &(c->camera_pos.z));
    else if (strcmp(key, "camera_target") == 0)
		sscanf(value, "%f %f %f", &(c->camera_target.x), &(c->camera_target.y), &(c->camera_target.z));
	else if (strcmp(key, "scale") == 0)
		sscanf(value, "%f", &(c->scale));
	else if (strcmp(key, "bailout") ==0)
		sscanf(value, "%d", &(c->bailout));
	else if (strcmp(key, "width") == 0)
		sscanf(value, "%d", &(c->width));
	else if (strcmp(key, "height") == 0)
		sscanf(value, "%d", &(c->height));
    else if (strcmp(key, "fov") == 0)
        sscanf(value, "%f", &(c->fov));
    else if (strcmp(key, "normal_diff") == 0)
        sscanf(value, "%f", &(c->normal_diff));
    else if (strcmp(key, "light_pos") == 0)
        sscanf(value, "%f %f %f", &(c->light_pos.x), &(c->light_pos.y), &(c->light_pos.z));
    else if (strcmp(key, "output_file") == 0)
        sscanf(value, "%256s", &(c->output_file));
}

int load_config(char *filename, config *c)
{
    char buf[1024];
    char key[1024], value[1024];
    char *split;
    FILE *f = fopen(filename, "r");
    
    if (!f)
        return 0;
        
    while (!feof(f))
    {
        if (fgets(buf, sizeof(buf), f))
		{
			split = strchr(buf, ':');
			memset(key, 0, sizeof(key));
			strncpy(key, buf, split - buf);
			strcpy(value, split + 1);
			handle(key, value, c);
		}
    }
    
    fclose(f);

    return 1;
}
