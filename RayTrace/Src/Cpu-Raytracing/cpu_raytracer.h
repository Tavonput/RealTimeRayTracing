#pragma once

#include "Application/logging.h"


class CpuRaytracer
{
public:
	void init(uint32_t width, uint32_t height);
	void render();
	void cleanup();
	int calculateSpace(int width, int height);
	
	int progress = 0;
	int height = 0;
	int width = 0;
	bool debug = false;
	struct vector {
		double v1 = 0;
		double v2 = 0;
		double v3 = 0;
	};
	void vecotrInit(struct vector & vec);
	double vecVertical();
	double vecHorizontal();
private:
};

double reflectance(double x, double y);

double degreeToRad(double d);

bool raycolor();

double random_double();

double randomRangedDouble(double x ,double y);

double viewpoint_height(double height, double focal);

