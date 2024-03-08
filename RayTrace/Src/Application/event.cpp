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

std::string WindowMinimizedEvent::eventString() const
{
	return "Window Minimized";
}

std::string MouseClickEvent::eventString() const
{
	std::string buttonStr = (button == GLFW_MOUSE_BUTTON_LEFT) ? "LEFT" : "RIGHT";
	std::string shift = (mods & GLFW_MOD_SHIFT) ? "SHIFT" : "NONE";

	std::stringstream ss;
	ss << "Mouse Click - " << buttonStr << ", mod: " << shift;
	return ss.str();
}

std::string MouseReleaseEvent::eventString() const
{
	std::string buttonStr = (button == GLFW_MOUSE_BUTTON_LEFT) ? "LEFT" : "RIGHT";
	std::string shift = (mods & GLFW_MOD_SHIFT) ? "SHIFT" : "NONE";

	std::stringstream ss;
	ss << "Mouse Release - " << buttonStr << ", mod: " << shift;
	return ss.str();
}

std::string MouseMoveEvent::eventString() const
{
	std::stringstream ss;
	ss << "Mouse Move - x:" << xPos << ", y:" << yPos;
	return ss.str();
}

std::string KeyPressEvent::eventString() const
{
	std::string keyStr;
	if      (key == GLFW_KEY_W)  keyStr = "W";
	else if (key == GLFW_KEY_A)  keyStr = "A";
	else if (key == GLFW_KEY_S)  keyStr = "S";
	else if (key == GLFW_KEY_D)  keyStr = "D";

	std::stringstream ss;
	ss << "Key Press - " << keyStr;
	return ss.str();
}

std::string KeyReleaseEvent::eventString() const
{
	std::string keyStr;
	if      (key == GLFW_KEY_W)  keyStr = "W";
	else if (key == GLFW_KEY_A)  keyStr = "A";
	else if (key == GLFW_KEY_S)  keyStr = "S";
	else if (key == GLFW_KEY_D)  keyStr = "D";

	std::stringstream ss;
	ss << "Key Release - " << keyStr;
	return ss.str();
}