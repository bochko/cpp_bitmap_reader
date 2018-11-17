#include <cstdint>
#include <iostream>

/// Boyan M. Atanasov
/// 2018
///
/// version: 0.1 reads 24 bit bitmaps

#ifdef _MSC_VER

#define __packed__structure__(__structure__) __pragma( pack(push, 1) ); __structure__ __pragma( pack(pop) );

#define IMPL_FOPEN(fname, mode) fopen(fname, mode)
#define IMPL_FCLOSE(fptr) fclose(fptr)
#define IMPL_FREAD(buffer_ptr, elsize, elnum, fptr) fread(buffer_ptr, elsize, elnum, fptr)
#define IMPL_FSEEK(fptr, offset, origin) fseek(fptr, offset, origin)
#define IMPL_FILE FILE*

#elif __GNUC__

#define __packed__structure__(__structure__) __structure__ __attribute__((packed), aligned(1))

#define IMPL_FOPEN(fname, mode) fopen(fname, mode)
#define IMPL_FCLOSE(fptr) fclose(fptr)
#define IMPL_FREAD(buffer_ptr, elsize, elnum, fptr) fread(buffer_ptr, elsize, elnum, fptr)
#define IMPL_FSEEK(fptr, offset, origin) fseek(fptr, offset, origin)
#define IMPL_FILE int

#else // this should be edited per platform

#define __packed__structure__(__structure__)

#define IMPL_FOPEN
#define IMPL_FCLOSE
#define IMPL_FREAD
#define IMPL_FSEEK
#define IMPL_FILE

#endif

#define BITMAP_MAGVAL 0x4D42

__packed__structure__(
	typedef struct {
	uint16_t magic; // magic file identifier
	uint32_t size; // file size
	uint16_t reserved1; // 0
	uint16_t reserved2; // 0
	uint32_t offset_imgdat; // image data offset
} bitmap_header_t;
)

__packed__structure__(
	typedef struct {
	uint32_t size;  // header size bytes
	uint32_t width; // width of image
	uint32_t height; // height of image
	uint16_t planes; // number of planes
	uint16_t bits; // bits per pixel
	uint32_t compression; // compression
	uint32_t imagesize; // image data size in bytes
	uint32_t xresolution; // x pixels per meter
	uint32_t yresolution; // y pixels per meter
	uint32_t ncolours; // number of colours
	uint32_t importantcolours; // important colours
} bitmap_info_header_t;
)

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