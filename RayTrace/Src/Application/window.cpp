#include "pch.h"

#include "window.h"
#include "imgui.h"

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
	glfwSetKeyCallback(m_window, &keyCallback);

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

bool Window::isWindowClosed()
{
	return glfwWindowShouldClose(m_window);
}

bool Window::isWindowMinimized()
{
	int width, height;
	glfwGetWindowSize(m_window, &width, &height);

	if (width == 0 || height == 0)
		Sleep(50);
	else
		m_minimized = false;

	return m_minimized;
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto& queue = EventDispatcher::GetEventQueue();

	if (width == 0 || height == 0)
		queue.push_back(std::make_unique<WindowMinimizedEvent>());
	else
		queue.insert(queue.begin(), std::make_unique<WindowResizeEvent>(width, height));
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	// ImGui event
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	if (action == GLFW_PRESS)
		EventDispatcher::GetEventQueue().push_back(std::make_unique<MouseClickEvent>(button, mods));
	else
		EventDispatcher::GetEventQueue().push_back(std::make_unique<MouseReleaseEvent>(button, mods));
}

void Window::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	EventDispatcher::GetEventQueue().push_back(std::make_unique<MouseMoveEvent>(static_cast<int>(xpos), static_cast<int>(ypos)));
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	std::vector<int> keys = { 
		GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_U,
		GLFW_KEY_ESCAPE, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_SPACE, GLFW_KEY_LEFT_CONTROL }; // Keys to care about

	for (int i = 0; i < keys.size(); i++) //Loops through keys to find match
	{
		if (key == keys[i] && action == GLFW_PRESS) // Key press
			EventDispatcher::GetEventQueue().push_back(std::make_unique<KeyPressEvent>(key, mods));
		else if (key == keys[i] && action == GLFW_RELEASE) // Key release
			EventDispatcher::GetEventQueue().push_back(std::make_unique<KeyReleaseEvent>(key, mods));
	}
}

void Window::cleanup()
{
	APP_LOG_INFO("Destroying window and GLFW");

	glfwDestroyWindow(m_window);
	glfwTerminate();
}
