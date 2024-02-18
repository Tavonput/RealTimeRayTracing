#include "pch.h"

#include "window.h"

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

	// Set callbacks
	glfwSetFramebufferSizeCallback(m_window, &framebufferResizeCallback);
	glfwSetMouseButtonCallback(m_window, &mouseButtonCallback);
	glfwSetCursorPosCallback(m_window, &cursorPositionCallback);

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

bool Window::isWindowClosed()
{
	return glfwWindowShouldClose(m_window);
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	// Insert into the beginning to act as FIFO
	auto& queue = EventDispatcher::GetEventQueue();
	queue.insert(queue.begin(), std::make_unique<WindowResizeEvent>(width, height));
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		EventDispatcher::GetEventQueue().push_back(std::make_unique<MouseClickEvent>(button, mods));
	else
		EventDispatcher::GetEventQueue().push_back(std::make_unique<MouseReleaseEvent>(button, mods));
}

void Window::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	EventDispatcher::GetEventQueue().push_back(std::make_unique<MouseMoveEvent>(static_cast<int>(xpos), static_cast<int>(ypos)));
}

void Window::cleanup()
{
	APP_LOG_INFO("Destroying window and GLFW");

	glfwDestroyWindow(m_window);
	glfwTerminate();
}
