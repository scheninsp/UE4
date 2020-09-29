#include<iostream>
#include<fstream>
#include<string>
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
	if (!in.is_open()) { std::cerr << "open failed\n" << std::endl; in.close(); return false; }

	//read header
	TGA_Header header;
	in.read(reinterpret_cast<char*> (&header), sizeof(header));  //(&header)
	if (!in.good()) { std::cerr << "read header failed\n" << std::endl; in.close(); return false; }

	//read data
	width = header.width;
	height = header.height;
	bytespp = header.bitsperpixel>>3;  // >>3 = /8

	if (width <= 0 || height <= 0 || (bytespp != GRAYSCALE || bytespp != RGB || bytespp != RGBA)) {
		std::cerr << "image format error\n" << std::endl; in.close(); return false;
	}

	size_t nbytes = bytespp * width * height;
	data = std::vector<std::uint8_t>(nbytes, 0);

	if (3 == header.datatypecode || 2 == header.datatypecode) {   //unzipped
		in.read(reinterpret_cast<char*>(data.data()), nbytes);
		if (!in.good()) { std::cerr << "data read failed\n" << std::endl; in.close(); return false; }
	}
	else if (10 == header.datatypecode || 11 == header.datatypecode) {   //zipped
		if (!load_rle_data(in)) {
			std::cerr << "data read failed\n" << std::endl; in.close(); return false;
		}
	}
	else {
		std::cerr << "data read error\n" << std::endl; in.close(); return false;
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
	size_t pixelcount = width * height;
	size_t currentpixel = 0;
	size_t currentbyte = 0;
	TGAColor colorbuffer;
	do {
		std::uint8_t chunkheader = 0;
		chunkheader = in.get();
		if (!in.good()) { std::cerr << "read chunkheader failed\n"; return false; }
		if (chunkheader < 128) {
			chunkheader++;
			for (int i = 0; i < chunkheader; i++) {

				in.read(reinterpret_cast<char *>(colorbuffer.bgra), bytespp);
				if (!in.good()) { std::cerr << "read data failed\n"; return false; }

				for (int t = 0; t < bytespp; t++) {
					data[currentbyte++] = colorbuffer.bgra[t];
				}
				currentpixel++;

				if (currentpixel > pixelcount) { std::cerr << "Too many pixels read\n"; return false; }
			}
		}
		else {
			chunkheader -= 127;  //read bgra once
			in.read(reinterpret_cast<char *>(colorbuffer.bgra), bytespp);
			if (!in.good()) { std::cerr << "read data failed\n"; return false; }

			for (int i = 0; i < chunkheader; i++) {

				for (int t = 0; t < bytespp; t++) {
					data[currentbyte++] = colorbuffer.bgra[t];
				}
				currentpixel++;

				if (currentpixel > pixelcount) { std::cerr << "Too many pixels read\n"; return false; }
			}
		}
	} while (currentpixel < pixelcount);
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


bool TypedImage::write_tga_file(const std::string filename, const bool vflip, const bool rle) const {
	std::uint8_t developer_area_ref[4] = { 0, 0, 0, 0 };
	std::uint8_t extension_area_ref[4] = { 0, 0, 0, 0 };
	std::uint8_t footer[18] = { 'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0' };
	std::ofstream out;
	out.open(filename, std::ios::binary);
	if (!out.is_open()) {
		std::cerr << "can't open file " << filename << "\n";
		out.close();
		return false;
	}
	TGA_Header header;
	header.bitsperpixel = bytespp << 3;
	header.width = width;
	header.height = height;
	header.datatypecode = (bytespp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
	header.imagedescriptor = vflip ? 0x00 : 0x20; // top-left or bottom-left origin
	out.write(reinterpret_cast<const char *>(&header), sizeof(header));
	if (!out.good()) {
		out.close();
		std::cerr << "can't dump the tga file\n";
		return false;
	}
	if (!rle) {
		out.write(reinterpret_cast<const char *>(data.data()), width*height*bytespp);
		if (!out.good()) {
			std::cerr << "can't unload raw data\n";
			out.close();
			return false;
		}
	}
	else {
		if (!unload_rle_data(out)) {
			out.close();
			std::cerr << "can't unload rle data\n";
			return false;
		}
	}
	out.write(reinterpret_cast<const char *>(developer_area_ref), sizeof(developer_area_ref));
	if (!out.good()) {
		std::cerr << "can't dump the tga file\n";
		out.close();
		return false;
	}
	out.write(reinterpret_cast<const char *>(extension_area_ref), sizeof(extension_area_ref));
	if (!out.good()) {
		std::cerr << "can't dump the tga file\n";
		out.close();
		return false;
	}
	out.write(reinterpret_cast<const char *>(footer), sizeof(footer));
	if (!out.good()) {
		std::cerr << "can't dump the tga file\n";
		out.close();
		return false;
	}
	out.close();
	return true;
}


bool TypedImage::unload_rle_data(std::ofstream &out) const {
	const std::uint8_t max_chunk_length = 128;
	size_t npixels = width * height;
	size_t curpix = 0;
	while (curpix < npixels) {
		size_t chunkstart = curpix * bytespp;
		size_t curbyte = curpix * bytespp;
		std::uint8_t run_length = 1;
		bool raw = true;
		while (curpix + run_length < npixels && run_length < max_chunk_length) {
			bool succ_eq = true;
			for (int t = 0; succ_eq && t < bytespp; t++)
				succ_eq = (data[curbyte + t] == data[curbyte + t + bytespp]);
			curbyte += bytespp;
			if (1 == run_length)
				raw = !succ_eq;
			if (raw && succ_eq) {
				run_length--;
				break;
			}
			if (!raw && !succ_eq)
				break;
			run_length++;
		}
		curpix += run_length;
		out.put(raw ? run_length - 1 : run_length + 127);
		if (!out.good()) {
			std::cerr << "can't dump the tga file\n";
			return false;
		}
		out.write(reinterpret_cast<const char *>(data.data() + chunkstart), (raw ? run_length * bytespp : bytespp));
		if (!out.good()) {
			std::cerr << "can't dump the tga file\n";
			return false;
		}
	}
	return true;
}


