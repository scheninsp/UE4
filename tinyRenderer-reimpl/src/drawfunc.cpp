#include "drawfunc.h"

/* unoptimized 1 
void line(int x0, int y0, int x1, int y1, TypedImage &img, TGAColor c) {
	//to optimize : 
	//1 - border check

	bool steep = false;

	if (std::abs(y1-y0) > std::abs(x1-x0)) {   //transpose steep line to flat line
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}

	float t = (y1 - y0) / (float)(x1 - x0);
	float abst = std::abs(t);

	//always draw from left to right
	if (x1 < x0) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	float accum_t = 0;
	int y = y0;

	for (int i = x0; i <= x1; i++) {
		if (steep) {
				img.set(y, i, c);   //transpose back to original line
			}
		else {
				img.set(i, y, c);
		}

		//int y = y0 + t * (i - x0);
		//discretize y
		accum_t += abst;
		if (accum_t > 1) {   // >1 ? >0.5 ? no difference ? 
			y += (y1 > y0 ? 1 : -1);
			accum_t -= 1.;
		}

	}

};
*/

void line(int x0, int y0, int x1, int y1, TypedImage &img, TGAColor c) {
	//to optimize : 
	//1 - border check

	bool steep = false;

	if (std::abs(y1-y0) > std::abs(x1-x0)) {   //transpose steep line to flat line
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}

	//always draw from left to right
	if (x1 < x0) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dy = y1 - y0;
	int dx = x1 - x0;
	int abs_dy = std::abs(dy);
	int abs_dx = std::abs(dx);

	int accum_t_mul_2dx = 0;
	int y = y0;

	for (int i = x0; i <= x1; i++) {
		if (steep) {
			img.set(y, i, c);   //transpose back to original line
		}
		else {
			img.set(i, y, c);
		}

		//int y = y0 + t * (i - x0);
		//discretize y
		accum_t_mul_2dx += 2*abs_dy;
		if (accum_t_mul_2dx > abs_dx) {   // >1 ? >0.5 ? no difference ? 
			y += (y1 > y0 ? 1 : -1);
			accum_t_mul_2dx -= 2*abs_dx;
		}

	}

};


/* unoptimized 1 */
void triangle(vec2 t0, vec2 t1, vec2 t2, TypedImage &img, TGAColor c) {
	
	//sort t0 ~ t2 as y coordinate ascending order
	if (t0[1] > t1[1]) std::swap(t0, t1);
	if (t0[1] > t2[1]) std::swap(t0, t2);
	if (t1[1] > t2[1]) std::swap(t1, t2);

	//find cut-point y level of t1 - t2
	double t_cut_ylevel_y = t1[1];
	double upper_point_x = t2[0];
	double upper_point_y = t2[1];
	double t_cut_ylevel_x = (t0[0] - upper_point_x) / (t0[1] - upper_point_y) * (t_cut_ylevel_y - upper_point_y) + upper_point_x;
	
	double lower_point_x = t1[0];

	vec2 t_lower_left;
	vec2 t_lower_right;

	if (lower_point_x < t_cut_ylevel_x) {
		t_lower_left = vec2(lower_point_x, t_cut_ylevel_y);
		t_lower_right = vec2(t_cut_ylevel_x, t_cut_ylevel_y);
	}
	else {
		t_lower_right = vec2(lower_point_x, t_cut_ylevel_y);
		t_lower_left = vec2(t_cut_ylevel_x, t_cut_ylevel_y);
	}

	line(t0[0], t0[1], t_lower_left[0], t_lower_left[1], img, red);
	line(t0[0], t0[1], t_lower_right[0], t_lower_right[1], img, green);

	//fill between lines in lower part of triangle
	double kleft = (t0[0] - t_lower_left[0]) / (t0[1] - t_lower_left[1]);
	double kright = (t0[0] - t_lower_right[0]) / (t0[1] - t_lower_right[1]);

	for (int ty = t0[1]; ty <= t_cut_ylevel_y; ty++) {
		int tx_left = kleft * (ty - t_lower_left[1]) + t_lower_left[0];
		int tx_right = kright * (ty - t_lower_right[1]) + t_lower_right[0];
		line(tx_left, ty, tx_right, ty, img, white);
	}

	//fill upper part of triangle
	line(t2[0], t2[1], t_lower_left[0], t_lower_left[1], img, red);
	line(t2[0], t2[1], t_lower_right[0], t_lower_right[1], img, green);

	double kleft_upper = (t2[0] - t_lower_left[0]) / (t2[1] - t_lower_left[1]);
	double kright_upper = (t2[0] - t_lower_right[0]) / (t2[1] - t_lower_right[1]);

	for (int ty = t1[1]; ty <= t2[1]; ty++) {
		int tx_left = kleft_upper * (ty - t_lower_left[1]) + t_lower_left[0];
		int tx_right = kright_upper * (ty - t_lower_right[1]) + t_lower_right[0];
		line(tx_left, ty, tx_right, ty, img, white);
	}
};
