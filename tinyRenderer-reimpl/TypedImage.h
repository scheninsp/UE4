#ifndef __TYPEDIMAGE_H__
#define __TYPEDIMAGE_H__

#include<cstdint>
#include<vector>

class TypedImage {
protected:
	std::vector<std::uint8_t> data;
	int width, height;
	int bytespp;   //bytes per pixel

public:
	TypedImage();
	TypedImage(const int w, const int h, const int bytespp);
	std::uint8_t *buffer();
	void clear();
};

#endif