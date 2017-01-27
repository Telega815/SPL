#pragma once
#include "imagetk.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BI_RGB 0x0000

#pragma pack(push, 2)
typedef struct bmp_file_header_t
{
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
} bmp_file_header_t;

typedef struct bmp_info_header_t
{
	uint32_t biSize;
	uint32_t biWidth;
	uint32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	uint32_t biXPelsPerMeter;
	uint32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
} bmp_info_header_t;
#pragma pack(pop)

typedef enum
{
	READ_OK = 0,
	READ_UNSUCCESSFUL,
	READ_INVALID_FMT,
	READ_UNSUPPORTED_FMT
} bmp_read_error_code_t;

typedef enum
{
	WRITE_OK = 0,
	WRITE_INVALID_FMT,
	WRITE_UNSUCCESSFUL
} bmp_write_error_code_t;

bmp_read_error_code_t image_load_bmp(FILE *in, image_t *image);
bmp_write_error_code_t image_save_bmp(FILE *out, image_t *image);