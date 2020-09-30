#include<iostream>
#include<string>
#include "TypedImage.h"
#include "geometry.h"


int main() {
	
	const TGAColor red = TGAColor(255, 0, 0, 255);
	const TGAColor white = TGAColor(255, 255, 255, 255);

	TypedImage image(100, 100, TypedImage::RGB);
	//image.set(100-16, 16, red);

	//line(13, 20, 80, 40, image, white);
	line(20, 13, 40, 80, image, red);
	line(80, 40, 13, 20, image, red);

	image.flip_vertically();  //can be replaced by setting vflip=false in write_tga_file()
	image.write_tga_file("output_test.tga");

	return 0;
}