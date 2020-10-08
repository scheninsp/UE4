#include<iostream>
#include<string>
#include "TypedImage.h"
#include "drawfunc.h"
#include "model.h"

int main() {

	/* test1 draw lines 

	TypedImage image(100, 100, TypedImage::RGB);
	//image.set(100-16, 16, red);

	//line(13, 20, 80, 40, image, white);
	line(20, 13, 40, 80, image, red);
	line(80, 40, 13, 20, image, red);

	image.flip_vertically();  //can be replaced by setting vflip=false in write_tga_file()

	*/

	/* test2 wireframe 
	int width = 512;
	int height = 512;
	TypedImage image(width, height, TypedImage::RGB);

	std::string filename = "../obj/african_head/african_head.obj";
	Model* model = new Model(filename);

	for (int i = 0; i < model->nfaces(); i++) {
		for (int j = 0; j < 3; j++) {
			vec3 v0 = model->vert(i, j);
			vec3 v1 = model->vert(i, (j + 1) % 3);
			int x0 = (v0.x + 1.)*width / 2.;
			int y0 = (v0.y + 1.)*height / 2.;
			int x1 = (v1.x + 1.)*width / 2.;
			int y1 = (v1.y + 1.)*height / 2.;
			line(x0, y0, x1, y1, image, white);
		}
	}

	*/

	/* test3 triangle */

	int width = 256;
	int height = 256;
	TypedImage image(width, height, TypedImage::RGB);

	vec2 t0[3] = { vec2(10, 70),   vec2(50, 160),  vec2(70, 80) };
	vec2 t1[3] = { vec2(180, 50),  vec2(150, 1),   vec2(70, 180) };
	vec2 t2[3] = { vec2(180, 150), vec2(120, 160), vec2(130, 180) };
	triangle(t0[0], t0[1], t0[2], image, red);
	triangle(t1[0], t1[1], t1[2], image, white);
	triangle(t2[0], t2[1], t2[2], image, green);


	image.write_tga_file("output_test.tga");
	return 0;
}