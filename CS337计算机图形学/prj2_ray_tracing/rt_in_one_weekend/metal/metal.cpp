#include "pch.h"
#include "ray.h"
#include "vec3.h"
#include "hitable.h"
#include "hitable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include <iostream>
#include <stdlib.h>     
#include <time.h>
using namespace yph;
// cosnt 和 & 一定要注意起来，不要忘了各种位置的意义及区别

vec3<float> color(const ray<float>& r, hitable *world,int depth){
	hitRecord rec;
	if (world->hit(r, T_MIN, T_MAX, rec)) {			// 这里下限是为了防止shadow acne，即微小光线也漫反射，无穷无尽下去
		ray<float> scattered;
		vec3<float> attenuation;
		if (depth < 50 && rec.materialPtr->scatter(r,rec,attenuation,scattered)) {
			return attenuation * color(scattered, world, depth+1);		// 漫反射了0.5的。设置了一个迭代的最深深度，与之前的T_MIN一起作用
		}
		else {
			return vec3<float>(0, 0, 0);
		}
	}
	else {
		vec3<float> unitDirection = r.getDirection().makeUnitVector();
		float t = 0.5*(unitDirection.getY() + 1.0);						//限定在0到1
		return (1.0f - t)*vec3<float>(1.0, 1.0, 1.0) + t * vec3<float>(0.5, 0.7, 1.0);
	}
}
			
#define RAND ((rand() % 10) / 10.0)

hitable *randomScene() {
	int n = 500;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3<float>(0,-1000,0),1000,new lambertian(vec3<float>(0.5,0.5,0.5)));
	int i = 1;
	for (int a = -11; a < 11;++a) {
		for (int b = -11; b < 11; ++b) {
			float chooseMat = RAND;
			vec3<float> center(a + 0.9*RAND, 0.2, b + 0.9*RAND);
			if ((center-vec3<float>(4,0.2,0)).length()>0.9) {
				if (chooseMat<0.8) {
					list[i++] = new sphere(center, 0.2, new lambertian(vec3<float>(RAND*RAND, RAND*RAND, RAND*RAND)));
				}
				else if (chooseMat<0.95) {
					list[i++] = new sphere(center, 0.2, new metal(vec3<float>(0.5 * (1 + RAND), 0.5*(1 + RAND), 0.5*(1 + RAND)) , RAND));
				}
				else {
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}
	list[i++] = new sphere(vec3<float>(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3<float>(0, 1, 0), 1.0, new lambertian(vec3<float>(0.4,0.2,0.1)));
	list[i++] = new sphere(vec3<float>(0, 1, 0), 1.0, new metal(vec3<float>(0.7, 0.6, 0.5),0.0));
	return new hitableList(list, i);
}

int main()
{
	FILE *stream;
	srand(time(NULL));

	int nx = 1600;
	int ny = 800;
	int ns = 100;

	hitable* list[5];																// 指针数组
	list[0] = new sphere(vec3<float>(0, 0, -1), 0.5, new lambertian(vec3<float>(0.1, 0.2, 0.5)));
	list[1] = new sphere(vec3<float>(0, -100.5, -1), 100, new lambertian(vec3<float>(0.8, 0.8, 0.0)));
	list[2] = new sphere(vec3<float>(1, 0, -1), 0.5, new metal(vec3<float>(0.8, 0.6, 0.2)));	// 反射的清楚
	list[3] = new sphere(vec3<float>(-1, 0, -1), 0.5, new dielectric(1.5));
	list[4] = new sphere(vec3<float>(-1, 0, -1), -0.45, new dielectric(1.5));
	hitable* world = new hitableList(list, 5);
	world = randomScene();
	
	vec3<float> lookfrom(3, 3, 2);
	vec3<float> lookat(0, 0, -1);
	float distToFocus = (lookfrom - lookat).length();
	float aperture = 2.0;
	camera cam(lookfrom,lookat,vec3<float>(0,1,0),20,float(nx)/float(ny),aperture,distToFocus);

	freopen_s(&stream, "test.ppm", "w", stdout);
	std::cout << "P3\n" << nx << " " << ny << "\n255\n";
	for (int j = ny - 1; j >= 0; --j) {												 //从左上角开始绘制
		for (int i = 0; i < nx; ++i) {
			vec3<float> col(0, 0, 0);
			for (int s = 0; s < ns; ++s) {
				// 一个像素照射多次，每次随机产生一个偏差防止总是取一个像素的一个位置造成颜色剧变和锯齿，对颜色求和后再求平均值
				float u = float(i + (rand() % 10) / float(10)) / float(nx);
				float v = float(j + (rand() % 10) / float(10)) / float(ny);
				ray<float> r = cam.getRay(u, v);
				col += color(r, world, 0);
			}
			col /= float(ns);
			col = vec3<float>(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));			// 这个是做gamma矫正，视觉与相机的偏差
			int ir = int(col[0] * 255.99);
			int ig = int(col[1] * 255.99);
			int ib = int(col[2] * 255.99);
			std::cout << ir << " " << ig << " " << ib << std::endl;
		}
	}
}
