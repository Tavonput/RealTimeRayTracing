#include "pch.h"
#include "event.h"

// Define the static event queue
std::vector<std::unique_ptr<Event>> EventDispatcher::s_queue;

std::string WindowResizeEvent::eventString() const
{
	std::stringstream ss;
	ss << "Window Resize - Width: " << width << ", Height: " << height;
	return ss.str();
}

std::string MouseClickEvent::eventString() const
{
	std::string buttonStr = (button == GLFW_MOUSE_BUTTON_LEFT) ? "LEFT" : "RIGHT";

	std::stringstream ss;
	ss << "Mouse Click - " << buttonStr;
	return ss.str();
}

std::string MouseReleaseEvent::eventString() const
{
	std::string buttonStr = (button == GLFW_MOUSE_BUTTON_LEFT) ? "LEFT" : "RIGHT";

	std::stringstream ss;
	ss << "Mouse Release - " << buttonStr;
	return ss.str();
}

std::string MouseMoveEvent::eventString() const
{
	std::stringstream ss;
	ss << "Mouse Move - x:" << xPos << ", y:" << yPos;
	return ss.str();
}
