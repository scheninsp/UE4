#include "TypedImage.h"

TypedImage::TypedImage() : data(), width(0), height(0), bytespp(0) {}
TypedImage::TypedImage(const int w, const int h, const int bpp) : 
				width(w), height(h), bytespp(bpp), data(w * h * bpp, 0) {}

std::uint8_t *TypedImage::buffer() {
	return data.data();
}

void TypedImage::clear() {
	data = std::vector<std::uint8_t>(width*height*bytespp, 0);
}