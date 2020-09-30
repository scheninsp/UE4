#ifndef __TYPEDIMAGE_H__
#define __TYPEDIMAGE_H__

#include<cstdint>
#include<vector>
#include<algorithm>  //std::min, std::max

#pragma pack(push,1)
struct TGA_Header {
	std::uint8_t  idlength{};
	std::uint8_t  colormaptype{};
	std::uint8_t  datatypecode{};
	std::uint16_t colormaporigin{};
	std::uint16_t colormaplength{};
	std::uint8_t  colormapdepth{};
	std::uint16_t x_origin{};
	std::uint16_t y_origin{};
	std::uint16_t width{};
	std::uint16_t height{};
	std::uint8_t  bitsperpixel{};
	std::uint8_t  imagedescriptor{};
};
#pragma pack(pop)

struct TGAColor {
	std::uint8_t bgra[4] = { 0,0,0,0 };
	std::uint8_t bytespp = { 0 };

	TGAColor() = default;
	TGAColor(const std::uint8_t R, const std::uint8_t G, const std::uint8_t B, const std::uint8_t A = 255) : bgra{ B,G,R,A }, bytespp(4) {};
	TGAColor(const std::uint8_t* p, const std::uint8_t bpp) : bytespp(bpp) {
		for (int i = 0; i < bpp; i++) {
			bgra[i] = p[i];
		}
	};

	std::uint8_t& operator[](const int i) { return bgra[i]; };

	TGAColor& operator* (const double intensity) const {
	//(0,1) to (0, intensity)
		TGAColor res = *this;
		double resonable_intensity = std::max(0.0, std::min(intensity, 1.0));
		for (int i = 0; i < 4; i++) {
			res.bgra[i] = static_cast<uint8_t>(res.bgra[i] * resonable_intensity);
		}
		return res;
	}

};


class TypedImage {
protected:
	std::vector<std::uint8_t> data;
	int width, height;
	int bytespp;   //bytes per pixel

	bool load_rle_data(std::ifstream &in);
	bool unload_rle_data(std::ofstream &out) const;

public:
	enum Format{GRAYSCALE=1, RGB=3, RGBA=4};  //bytespp of each format

	TypedImage();
	TypedImage(const int w, const int h, const int bytespp);
	std::uint8_t *buffer();
	void clear();

	bool read_tga_file(const std::string filename);

	void flip_vertically();
	void flip_horizontally();

	TGAColor get(const int col, const int row) const;
	void set(const int col, const int row, const TGAColor &c);

	bool write_tga_file(const std::string filename, const bool vflip=true, const bool rle=true) const;

};

#endif