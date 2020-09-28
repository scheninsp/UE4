#include<iostream>
#include<fstream>
#include "TypedImage.h"

TypedImage::TypedImage() : data(), width(0), height(0), bytespp(0) {}
TypedImage::TypedImage(const int w, const int h, const int bpp) : 
				width(w), height(h), bytespp(bpp), data(w * h * bpp, 0) {}

TGAColor TypedImage::get(const int col, const int row) const {
	if (!data.size() || row < 0 || row >= height || col<0 || col>width) { return {}; }

	return TGAColor(data.data() + (col + row * width)*bytespp, bytespp);
};

void TypedImage::set(const int col, const int row, const TGAColor &c) {
	if (!data.size() || row < 0 || row >= height || col<0 || col>width) { return; }

	memcpy(data.data() + (col + row * width)*bytespp, c.bgra, bytespp) ;   //data saved as bgra
};

std::uint8_t *TypedImage::buffer() {
	return data.data();
}

void TypedImage::clear() {
	data = std::vector<std::uint8_t>(width*height*bytespp, 0);
}

bool TypedImage::read_tga_file(const std::string filename) {
	std::ifstream in;
	in.open(filename, std::ios::binary);
	if (!in.is_open()) { std::cerr << "open failed" << std::endl; in.close(); return false; }

	//read header
	TGA_Header header;
	in.read(reinterpret_cast<char*> (&header), sizeof(header));  //(&header)
	if (!in.good()) { std::cerr << "read header failed" << std::endl; in.close(); return false; }

	//read data
	width = header.width;
	height = header.height;
	bytespp = header.bitsperpixel>>3;  // >>3 = /8

	if (width <= 0 || height <= 0 || (bytespp != GRAYSCALE || bytespp != RGB || bytespp != RGBA)) {
		std::cerr << "image format error" << std::endl; in.close(); return false;
	}

	size_t nbytes = bytespp * width * height;
	data = std::vector<std::uint8_t>(nbytes, 0);

	if (3 == header.datatypecode || 2 == header.datatypecode) {   //unzipped
		in.read(reinterpret_cast<char*>(data.data()), nbytes);
		if (!in.good()) { std::cerr << "data read failed" << std::endl; in.close(); return false; }
	}
	else if (10 == header.datatypecode || 11 == header.datatypecode) {   //zipped
		if (!load_rle_data(in)) {
			std::cerr << "data read failed" << std::endl; in.close(); return false;
		}
	}
	else {
		std::cerr << "data read error" << std::endl; in.close(); return false;
	}


	//read imagedescription
	if (!(header.imagedescriptor & 0x20))
		flip_vertically();
	if (header.imagedescriptor & 0x10)
		flip_horizontally();
	std::cerr << width << "x" << height << "/" << bytespp * 8 << "\n";
	
	in.close();
	return true;
}


bool TypedImage::load_rle_data(std::ifstream &in) {
	return true;
}

void TypedImage::flip_vertically() {
	if (data.size() == 0) { return; }

	size_t bytes_per_row = width * bytespp;
	std::vector<std::uint8_t> row(bytes_per_row, 0);

	int height_2 = height >> 1;

	for (int i = 0; i < height_2; i++) {
		size_t st_l1 = i * bytes_per_row;
		size_t st_l2 = (height - i - 1) * bytes_per_row;

		//data is save in row priority
		std::copy(data.begin() + st_l1, data.begin() + st_l1 + bytes_per_row, row.begin());
		std::copy(data.begin() + st_l2, data.begin() + st_l2 + bytes_per_row, data.begin()+st_l1);
		std::copy(row.begin(), row.begin() + bytes_per_row, data.begin()+st_l2);
	}
}

void TypedImage::flip_horizontally() {
	if (!data.size()) return;

	int width_2 = width >> 1;
	//why must construct a TGAColor object, instead of direct memory access?
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width_2; j++) {
			TGAColor c1 = get(j, i);
			TGAColor c2 = get(width - 1 - j, i);
			set(j, i, c2);
			set(width - 1 - j, i, c1);
		}
	}

}
