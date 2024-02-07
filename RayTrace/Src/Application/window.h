#pragma once

#include <GLFW/glfw3.h> 

#include "logging.h"

class Window
{
public:
	bool framebufferResized = false;

	// Initializer
	void init(uint32_t width, uint32_t height);

	// Getters
	GLFWwindow* getWindowGLFW() const;
	void getSize(int* width, int* height) const;

	void resetFramebufferResize();

	// Cleanup
	void cleanup();

private:
	GLFWwindow* m_window = nullptr;
};