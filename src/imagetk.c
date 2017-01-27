#include "imagetk.h"

void *memcpy2 (uint8_t *destination, const uint8_t *source, int n){
	for (int i = 0; i < n; i++){
		*(destination++) = *(source++);
	}
}

image_t* image_new(char *pixel_fmt, const uint8_t bpp, const uint32_t width, const uint32_t height, uint8_t *data)
{
	image_t *new = malloc(sizeof(image_t));

	new->pixel_fmt = malloc(strlen(pixel_fmt));
	strcpy(new->pixel_fmt, pixel_fmt);

	new->width = width;
	new->height = height;

	new->bpp = bpp;
	new->data = malloc(width * height * (bpp / 3));

	return new;
}

void image_free(image_t *image)
{
	free(image->pixel_fmt);
	free(image->data);
	free(image);
}

/* rotation by sampling */
void image_rotate(image_t *image, const float angle)
{
	int x, y, x_norm, y_norm, x_new, y_new, width_new, height_new;
	float a_cos = cos(angle), a_sin = sin(angle);
	uint8_t *data_new;

	width_new = image->width * fabsf(a_cos) + image->height * fabsf(a_sin);
	height_new = image->width * fabsf(a_sin) + image->height * fabsf(a_cos);
	data_new = (uint8_t*) malloc(width_new * height_new * (image->bpp / 8));
	memset(data_new, 255, width_new * height_new * (image->bpp / 8));

	for (y = 0; y < height_new; y++)
	{
		for (x = 0; x < width_new; x++)
		{
			x_norm = x - width_new / 2;
			y_norm = y - height_new / 2;
			x_new = x_norm * a_cos - y_norm * a_sin + image->width / 2;
			y_new = x_norm * a_sin + y_norm * a_cos + image->height / 2;
			if (x_new >= image->width || y_new >= image->height) continue;
			/*
			memcpy(data_new + y * width_new * (image->bpp / 8) + x * (image->bpp / 8),
				image->data + y_new * image->width * (image->bpp / 8) + x_new * (image->bpp / 8), image->bpp / 8);*/
			memcpy2(data_new + y * width_new * (image->bpp / 8) + x * (image->bpp / 8), image->data + y_new * image->width * (image->bpp / 8) + x_new * (image->bpp / 8), 3);
		}
	}

	free(image->data);
	image->width = width_new;
	image->height = height_new;
	image->data = data_new;
}

void image_blur_average(image_t *image, const int radius_h, const int radius_v)
{
	int x, y, i, avg_r_x, avg_g_x, avg_b_x, avg_r_y, avg_g_y, avg_b_y;
	uint8_t *buffer;
	rgb24_pixel_t *source_p, *buffer_p;

	if (radius_h < 0 || radius_v < 0) return;

	if (strcmp(image->pixel_fmt, "RGB") != 0) return;
	if (image->bpp != 24) return;

	buffer = (uint8_t*) malloc(image->height * image->width * sizeof(rgb24_pixel_t));
	memcpy2(buffer, image->data, image->height * image->width * sizeof(rgb24_pixel_t));

	for (x = 0; x < image->width; x++)
	{
		source_p = (rgb24_pixel_t*) image->data + x;
		buffer_p = (rgb24_pixel_t*) buffer + x;

		for (y = 0; y < image->height; y++)
		{
			if (y == 0)
			{
				avg_r_y = 0; avg_g_y = 0; avg_b_y = 0;
				for (i = 0; i <= radius_v; i++)
				{
					avg_r_y += (buffer_p + (int) (i * image->width))->r;
					avg_g_y += (buffer_p + (int) (i * image->width))->g;
					avg_b_y += (buffer_p + (int) (i * image->width))->b;
				}
				source_p->r = avg_r_y / (radius_v + 1);
				source_p->g = avg_g_y / (radius_v + 1);
				source_p->b = avg_b_y / (radius_v + 1);	
			}
			else if (y <= radius_v)
			{
				avg_r_y += (buffer_p + radius_v * image->width)->r;
				avg_g_y += (buffer_p + radius_v * image->width)->g;
				avg_b_y += (buffer_p + radius_v * image->width)->b;
				source_p->r = avg_r_y / (y + radius_v + 1);
				source_p->g = avg_g_y / (y + radius_v + 1);
				source_p->b = avg_b_y / (y + radius_v + 1);
			}
			else if (y < image->height - radius_v)
			{
				avg_r_y += (buffer_p + radius_v * image->width)->r - (buffer_p - (radius_v + 1) * image->width)->r;
				avg_g_y += (buffer_p + radius_v * image->width)->g - (buffer_p - (radius_v + 1) * image->width)->g;
				avg_b_y += (buffer_p + radius_v * image->width)->b - (buffer_p - (radius_v + 1) * image->width)->b;
				source_p->r = avg_r_y / (2 * radius_v + 1);
				source_p->g = avg_g_y / (2 * radius_v + 1);
				source_p->b = avg_b_y / (2 * radius_v + 1);
			}
			else
			{
				avg_r_y -= (buffer_p - radius_v * image->width)->r;
				avg_g_y -= (buffer_p - radius_v * image->width)->g;
				avg_b_y -= (buffer_p - radius_v * image->width)->b;
				source_p->r = avg_r_y / (radius_v + image->height - y);
				source_p->g = avg_g_y / (radius_v + image->height - y);
				source_p->b = avg_b_y / (radius_v + image->height - y);
			}

			source_p += image->width;
			buffer_p += image->width;
		}
	}

	memcpy2(buffer, image->data, image->height * image->width * sizeof(rgb24_pixel_t));

	for (y = 0; y < image->height; y++)
	{
		source_p = (rgb24_pixel_t*) image->data + y * image->width;
		buffer_p = (rgb24_pixel_t*) buffer + y * image->width;

		for (x = 0; x < image->width; x++)
		{
			if (x == 0)
			{
				avg_r_x = 0; avg_g_x = 0; avg_b_x = 0;
				for (i = 0; i <= radius_h; i++)
				{
					avg_r_x += (buffer_p + i)->r;
					avg_g_x += (buffer_p + i)->g;
					avg_b_x += (buffer_p + i)->b;
				}
				source_p->r = avg_r_x / (radius_h + 1);
				source_p->g = avg_g_x / (radius_h + 1);
				source_p->b = avg_b_x / (radius_h + 1);
			}
			else if (x <= radius_h)
			{
				avg_r_x += (buffer_p + radius_h)->r;
				avg_g_x += (buffer_p + radius_h)->g;
				avg_b_x += (buffer_p + radius_h)->b;
				source_p->r = avg_r_x / (x + radius_h + 1);
				source_p->g = avg_g_x / (x + radius_h + 1);
				source_p->b = avg_b_x / (x + radius_h + 1);
			}
			else if (x < image->width - radius_h)
			{
				avg_r_x += (buffer_p + radius_h)->r - (buffer_p - radius_h - 1)->r;
				avg_g_x += (buffer_p + radius_h)->g - (buffer_p - radius_h - 1)->g;
				avg_b_x += (buffer_p + radius_h)->b - (buffer_p - radius_h - 1)->b;
				source_p->r = avg_r_x / (2 * radius_h + 1);
				source_p->g = avg_g_x / (2 * radius_h + 1);
				source_p->b = avg_b_x / (2 * radius_h + 1);
			}
			else
			{
				avg_r_x -= (buffer_p - radius_h)->r;
				avg_g_x -= (buffer_p - radius_h)->g;
				avg_b_x -= (buffer_p - radius_h)->b;
				source_p->r = avg_r_x / (radius_h + image->width - x);
				source_p->g = avg_g_x / (radius_h + image->width - x);
				source_p->b = avg_b_x / (radius_h + image->width - x);
			}

			source_p++;
			buffer_p++;
		}
	}

	free(buffer);
}

void image_blur_fgaussian(image_t *image, const int radius)
{
	if (radius < 3) { image_blur_average(image, radius, radius); return; }
	image_blur_average(image, radius / 3, radius / 3);
	image_blur_average(image, radius / 3, radius / 3);
	image_blur_average(image, radius / 3 + radius % 3, radius / 3 + radius % 3);
}
