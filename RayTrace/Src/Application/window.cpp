#include "pch.h"

#include "window.h"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	// Callback on a window resize
	auto context = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	context->framebufferResized = true;
}

void Window::init(uint32_t width, uint32_t height)
{
	APP_LOG_INFO("Initializing GLFW");

	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// Create window
	m_window = glfwCreateWindow(width, height, "Real Time Raytracing", nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);

	// The window will show up 100 pixels off the top left of the monitor
	glfwSetWindowPos(m_window, 100, 100);

	// Set framebuffer resize callback
	glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);

	APP_LOG_INFO("GLFW initialization successful");
}

GLFWwindow* Window::getWindowGLFW() const
{
	return m_window;
}

void Window::getSize(int* width, int* height) const
{
	glfwGetFramebufferSize(m_window, width, height);
}

void Window::resetFramebufferResize()
{
	framebufferResized = false;
}

void Window::cleanup()
{
	APP_LOG_INFO("Destroying window and GLFW");

	glfwDestroyWindow(m_window);
	glfwTerminate();
}
