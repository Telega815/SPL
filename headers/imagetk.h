#pragma once
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct rgb24_pixel_t
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} rgb24_pixel_t;

typedef struct image_t
{
	char *pixel_fmt;
	uint32_t width;
	uint32_t height;
	uint8_t bpp;
	uint8_t *data;
} image_t;

image_t* image_new(char *pixel_fmt, const uint8_t bpp, const uint32_t width, const uint32_t height, uint8_t *data);
void image_free(image_t *image);

void image_rotate(image_t *image, const float angle);
void image_blur_average(image_t *image, const int radius_h, const int radius_v);
void image_blur_fgaussian(image_t *image, const int radius);