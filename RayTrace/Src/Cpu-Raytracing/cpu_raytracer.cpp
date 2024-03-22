#include "pch.h"
#include "cpu_raytracer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "stb_image_usage.h"



int CpuRaytracer::calculateSpace(int width, int height) {

	return width * height * 3;
}



void CpuRaytracer::vecotrInit(vector& vec)
{
}

double CpuRaytracer::vecVertical()
{
	return 0.0;
}

double CpuRaytracer::vecHorizontal()
{
	return 0.0;
}

void CpuRaytracer::init(uint32_t width, uint32_t height)
{
	APP_LOG_INFO("Initialize CPU raytracer");
	APP_LOG_INFO(width);
	APP_LOG_INFO(height);
	int inWidth = width;
	const int total = calculateSpace(width, height);
	char* imageData = new char[total];
	int pos = 0;
	APP_LOG_INFO("Steps remaining:");
	for (int xx = 0; xx < height; ++xx) {
		if(xx%100==0) APP_LOG_INFO(height - xx);
		for (int yy = 0; yy < width; ++yy) {
			double red = double(yy) / (width - 1);
			double green = double(xx) / (height - 1);
			double blue = 1;

			int iRed = static_cast<int>(255.999 * red);
			int igreen = static_cast<int>(255.999 * green);
			int iblue = static_cast<int>(255.999 * blue);
			imageData[pos++] = iRed;
			imageData[pos++] = igreen;
			imageData[pos++] = iblue;
		}
	}
	stbi_write_png("cpuRayTraceObject.png",width, height,3,imageData,width*3);
	auto viewpointheight = 1.0;
	auto viewpointwidth = viewpointheight * (double)width / height;
	free(imageData);
}

void CpuRaytracer::render()
{
	APP_LOG_INFO("Render CPU raytraced scene");
	glm::vec3 inital = {0.0f, 0.0f, 0.0f};
	// NOTES: 
	// Consider using stb_image to save the result as a png or something. The function is stbi_write_png()
	// Consider using glm for linear algebra stuff.
}

void CpuRaytracer::cleanup()
{
	APP_LOG_INFO("Destroying CPU raytracer");
}

double reflectance(double x, double y)
{
	return 0.0;
}

double degreeToRad(double d)
{
	return d * 3.14159 / 180.0;
}

double random_double()
{
	return rand() / (RAND_MAX + 1.0);
}

double randomRangedDouble(double x, double y)
{
	return 0.0;
}

double viewpoint_height(double height, double focal)
{
	return 0.0;
}



