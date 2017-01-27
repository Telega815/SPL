#include "imagetk.h"
#include "imagetk_bmp.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


int main(int argc, char **argv)
{
	FILE *test_in;
	FILE *test_out;

	image_t *image = image_new("", 0, 0, 0, NULL);

	if (argc < 1) return 1;

	test_in = fopen(argv[1], "rb");
	test_out = fopen("out.bmp", "wb");


	image_load_bmp(test_in, image);

	image_rotate(image, M_PI/4);
	image_blur_fgaussian(image, 5);

	if(image_save_bmp(test_out, image) == WRITE_OK){
		printf("%s\n", "its fine");
	}

	fclose(test_out);
	fclose(test_in);

	return 0;
}