#pragma once

#include <GLFW/glfw3.h> 

#include "logging.h"
#include "event.h"

class Window
{
public:
	bool framebufferResized = false;

	// Initializer
	void init(uint32_t width, uint32_t height);

	// Getters
	GLFWwindow* getWindowGLFW() const;
	void getSize(int* width, int* height) const;

	bool isWindowClosed();
	bool isWindowMinimized();

	// Callbacks
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

	void onWindowMinimized() { m_minimized = true; }

	// Cleanup
	void cleanup();

private:
	GLFWwindow* m_window = nullptr;

	bool m_minimized = false;
};