#ifndef __DRAWFUNC_H__
#define __DRAWFUNC_H__

#include "geometry.h"
#include "TypedImage.h"

void line(int x0, int y0, int x1, int y1, TypedImage &img, TGAColor c);

void line(vec2 p1, vec2 p2, TypedImage &img, TGAColor c);

//void triangle(vec2 t0, vec2 t1, vec2 t2, TypedImage &img, TGAColor c);

void triangle(vec2 *pts, TypedImage &image, TGAColor c);

void triangle(vec3 *pts, float *zbuffer, TypedImage &image, TGAColor c);

vec3 world2screen(const vec3 world_coords, const int width, const int height) ;

#endif
