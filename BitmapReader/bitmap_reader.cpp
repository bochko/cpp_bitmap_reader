/// Boyan M. Atanasov
/// 2018
///
/// version: 0.1 reads 24 bit bitmaps

#include "bitmap_reader.h"

#include <cstdint>
#include <iostream>

uint8_t * load_bitmap_24bit(char * filename, bitmap_info_header_t * bitmap_info_ptr) {
	IMPL_FILE fptr;
	bitmap_header_t bitmap_header;
	uint8_t * imagedat;
	uint32_t image_idx = 0;
	uint8_t rgb_swap;

	fptr = IMPL_FOPEN(filename, "rb");
	if (fptr == nullptr) {
		return nullptr;
	}

	IMPL_FREAD(&bitmap_header, sizeof(bitmap_header_t), 1, fptr);
	if (bitmap_header.magic != BITMAP_MAGVAL) {
		IMPL_FCLOSE(fptr);
		return nullptr;
	}

	IMPL_FREAD(bitmap_info_ptr, sizeof(bitmap_info_header_t), 1, fptr);

	IMPL_FSEEK(fptr, bitmap_header.offset_imgdat, SEEK_SET);

	//imagedat = (uint8_t*)malloc(bitmap_info_ptr->imagesize);
	imagedat = new uint8_t[bitmap_info_ptr->imagesize];
	if (!imagedat) {
		delete imagedat;
		IMPL_FCLOSE(fptr);
		return nullptr;
	}

	IMPL_FREAD(imagedat, bitmap_info_ptr->imagesize, 1, fptr);

	// swap from BGR to RGB
	for (image_idx = 0; image_idx < bitmap_info_ptr->imagesize; image_idx += 3) {
		rgb_swap = imagedat[image_idx];
		imagedat[image_idx] = imagedat[image_idx + 2];
		imagedat[image_idx + 2] = rgb_swap;
	}

	IMPL_FCLOSE(fptr);
	return imagedat;
}

int main() {
	bitmap_info_header_t hdr;
	uint8_t * bitmap_data = load_bitmap_24bit((char*)"E:\\hive_mind_logo_1024.bmp", &hdr);
	return 0;
}