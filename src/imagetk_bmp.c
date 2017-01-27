#include "imagetk_bmp.h"

bmp_read_error_code_t image_load_bmp(FILE *in, image_t *image)
{
	bmp_file_header_t *file_header = malloc(sizeof(bmp_file_header_t));
	bmp_info_header_t *info_header = malloc(sizeof(bmp_info_header_t));
	int y, offset;
	bmp_read_error_code_t rc = READ_OK;

	rewind(in);
	if (fread(file_header, sizeof(bmp_file_header_t), 1, in) < 1)
	{
		rc = READ_UNSUCCESSFUL;
		goto finally;
	}
	if (file_header->bfType != 0x4d42) { rc = READ_INVALID_FMT; goto finally; }

	if (fread(info_header, sizeof(bmp_info_header_t), 1, in) < 1)
	{
		rc = READ_UNSUCCESSFUL;
		goto finally;
	}
	if (info_header->biSize != 40) { rc = READ_UNSUPPORTED_FMT; goto finally; }
	if (info_header->biHeight < 0) { rc = READ_UNSUPPORTED_FMT; goto finally; }
	if (info_header->biPlanes != 1) { rc = READ_INVALID_FMT; goto finally; }
	if (info_header->biBitCount != 24) { rc = READ_UNSUPPORTED_FMT; goto finally; }
	if (info_header->biCompression != BI_RGB) { rc = READ_UNSUPPORTED_FMT; goto finally; }

	image->pixel_fmt = "RGB";
	image->width = info_header->biWidth;
	image->height = info_header->biHeight;
	image->bpp = 24;
	image->data = (uint8_t*) malloc(sizeof(rgb24_pixel_t) * image->width * image->height);

	offset = info_header->biWidth * (info_header->biBitCount / 8) % 4;
	if (offset != 0) offset = 4 - offset;

	fseek(in, file_header->bfOffBits, SEEK_SET);
	for (y = info_header->biHeight - 1; y >= 0; --y)
	{
		if (fread((image->data + y * info_header->biWidth * sizeof(rgb24_pixel_t)), sizeof(rgb24_pixel_t), info_header->biWidth, in) < info_header->biWidth)
		{
			rc = READ_UNSUCCESSFUL;
			goto finally;
		}
		fseek(in, offset, SEEK_CUR);
	}

finally:
	free(info_header);
	free(file_header);

	return rc;
}

bmp_write_error_code_t image_save_bmp(FILE *out, image_t *image)
{
	bmp_file_header_t *file_header = malloc(sizeof(bmp_file_header_t));
	bmp_info_header_t *info_header = malloc(sizeof(bmp_info_header_t));
	bmp_write_error_code_t rc = WRITE_OK;
	int y, offset = image->width * sizeof(rgb24_pixel_t) % 4;
	if (offset != 0) offset = 4 - offset;

	if (strcmp(image->pixel_fmt, "RGB") != 0) { rc = WRITE_INVALID_FMT; goto finally; }
	if (image->bpp != 24) { rc = WRITE_INVALID_FMT; goto finally; }

	file_header->bfType = 0x4d42;
	file_header->bfSize = 54 + (image->width * sizeof(rgb24_pixel_t) + offset) * image->height;
	file_header->bfReserved1 = 0;
	file_header->bfReserved2 = 0;
	file_header->bfOffBits = 54;

	info_header->biSize = 40;
	info_header->biWidth = image->width;
	info_header->biHeight = image->height;
	info_header->biPlanes = 1;
	info_header->biBitCount = 24;
	info_header->biCompression = BI_RGB;
	info_header->biSizeImage = 0;
	info_header->biXPelsPerMeter = 2835;
	info_header->biYPelsPerMeter = 2835;
	info_header->biClrUsed = 0;
	info_header->biClrImportant = 0;

	rewind(out);
	fwrite(file_header, sizeof(bmp_file_header_t), 1, out);
	fwrite(info_header, sizeof(bmp_info_header_t), 1, out);
	for (y = image->height - 1; y >= 0; --y)
	{
		if (fwrite((image->data + y * image->width * sizeof(rgb24_pixel_t)), sizeof(rgb24_pixel_t), image->width, out) < image->width)
		{
			rc = WRITE_UNSUCCESSFUL;
			goto finally;
		}
		
		// stupid shit
		if (offset != 0)
		{
			if (fwrite(image->data, sizeof(uint8_t), offset, out) < offset)
			{
				rc = WRITE_UNSUCCESSFUL;
				goto finally;
			}
		}
	}

finally:
	free(file_header);
	free(info_header);

	return rc;
}