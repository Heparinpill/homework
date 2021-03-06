#include "pch.h"
#include "tclap/CmdLine.h"
#include "args.h"
#include "ray.h"
#include "vec3.h"
#include "hitable.h"
#include "hitable_list.h"
#include "triangle.h"
#include "camera.h"
#include "material.h"
#include <iostream>
#include <stdlib.h>     
#include <time.h>
#include <algorithm>
#include "scenes.h"

using namespace yph;

#define RAND ((rand() % 10) / 10.0)													// 取0-1的随机数

int maxDepth = 5;
bool background = true;

vec3f color(ray<float>& r, hitable *world, int depth) {
	hitRecord rec;
	if (world->hit(r, T_MIN, T_MAX, rec)) {											// 这里下限是为了防止shadow acne，即微小光线也漫反射，无穷无尽下去
		ray<float> scattered;
		vec3f attenuation;
		vec3f emitted = rec.materialPtr->emitted(rec.u, rec.v, rec.p);
		if (depth < maxDepth && rec.materialPtr->scatter(r, rec, attenuation, scattered)) {
			return emitted + attenuation * color(scattered, world, depth + 1);		// 漫反射了attenuation的。设置了一个迭代的最深深度，与之前的T_MIN一起作用
		}
		else {
			return emitted;
		}
	}
	else {
		if (background) {
			vec3<float> unitDirection = r.getDirection().makeUnitVector();
			float t = 0.5*(unitDirection.getY() + 1.0);									//限定在0到1
			return (1.0f - t)*vec3<float>(1.0, 1.0, 1.0) + t * vec3<float>(0.5, 0.7, 1.0);
		}
		else {
			return vec3f(0, 0, 0);
		}
	}
}


int main(int argc, char *argv[])
{
	FILE *stream;
	srand(time(NULL));
	rayTracingArgs *args = parseArgs(argc, argv);

	int nx = args->nx;
	int ny = args->ny;
	int ns = args->ns;
	int scene = args->scene;
	float vfov = args->vfov;
	int addFloor = args->addFloor;
	maxDepth = args->depth;
	background = args->background;
	std::string modelFile = args->model;
	std::string imageFile = args->image;

	hitableList *world;
	switch (scene)
	{
	case 1: {
		world = scene1(nx, ny, imageFile);
		break;
	}
	case 2: {
		world = scene2();
		break;
	}
	case 3: {
		world = scene3();
		break;
	}
	default: {
		world = scene4(modelFile);
		break;
	}
	}
	
	aabb sceneBox;
	world->boundingBox(0,0,sceneBox);
	camera *cam = cameraInit(sceneBox,scene,nx,ny,vfov);
	if (addFloor>0) {
		hitable *land = floorInit(sceneBox,addFloor);								// 防止过大的floor干扰相机创建
		world->push_back(land);
	}
	
	std::string outputFile = args->output + ".ppm";
	freopen_s(&stream, outputFile.c_str(), "w", stdout);
	std::cout << "P3\n" << nx << " " << ny << "\n255\n";
	for (int j = ny - 1; j >= 0; --j) {												//从左上角开始绘制
		for (int i = 0; i < nx; ++i) {
			vec3f col(0, 0, 0);
			for (int s = 0; s < ns; ++s) {									
				float u = float(i + RAND) / float(nx);								// 一个像素照射多次，每次随机产生一个偏差防止
				float v = float(j + RAND) / float(ny);								// 总是取一个像素的一个位置造成颜色剧变和锯齿，对颜色求和后再求平均值
				ray<float> r = cam->getRay(u, v);
				col += color(r, world, 0);
			}
			col /= float(ns);
			col = vec3f(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));					// 这个是做gamma矫正，视觉与相机的偏差
			int ir = int(std::min(col[0],1.0f) * 255.99);
			int ig = int(std::min(col[1],1.0f) * 255.99);
			int ib = int(std::min(col[2],1.0f) * 255.99);
			std::cout << ir << " " << ig << " " << ib << std::endl;
		}
	}
	fclose(stdout);
}
