#include "geometry.h"

void line(int x0, int y0, int x1, int y1, TypedImage &img, TGAColor c) {


	bool steep = false;

	float t = (float)(y1 - y0) / (float)(x1 - x0);

	if (std::abs(t) > 1) {   //transpose steep line to flat line
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
		t = 1 / t;
	}

	//always draw from left to right
	if (x1 < x0) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	if (steep) {
		for (int i = x0; i < x1; i++) {
			int y = y0 + t * (i - x0);
			img.set(y, i, c);   //transpose back to original line
		}
	}
	else {
		for (int i = x0; i < x1; i++) {
			int y = y0 + t * (i - x0);
			img.set(i, y, c);
		}
	}

};