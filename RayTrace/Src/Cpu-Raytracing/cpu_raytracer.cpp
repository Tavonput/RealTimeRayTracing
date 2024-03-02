#include "pch.h"
#include "cpu_raytracer.h"

void CpuRaytracer::init()
{
	APP_LOG_INFO("Initialize CPU raytracer");
}

void CpuRaytracer::render()
{
	APP_LOG_INFO("Render CPU raytraced scene");

	// NOTES: 
	// Consider using stb_image to save the result as a png or something.
	// Consider using glm for linear algebra stuff.
}

void CpuRaytracer::cleanup()
{
	APP_LOG_INFO("Destroying CPU raytracer");
}
