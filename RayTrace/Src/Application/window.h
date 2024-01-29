#pragma once

#include <GLFW/glfw3.h> 

#include "logging.h"

class Window
{
public:
	bool framebufferResized = false;

	void init(uint32_t width, uint32_t height, Logger logger);

	GLFWwindow* getWindow() const;
	void getSize(int* width, int* height) const;

	void cleanup();

private:
	Logger m_logger;

	GLFWwindow* m_window = nullptr;
};