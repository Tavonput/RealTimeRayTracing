#pragma once

#include <GLFW/glfw3.h>

// --------------------------------------------------------------------------
// Event
//

enum class EventType
{
	NONE = 0,
	WINDOW_RESIZE, WINDOW_MINIMIZED,
	MOUSE_CLICK, MOUSE_RELEASE, MOUSE_MOVE,
	KEY_PRESS, KEY_RELEASE
};

class Event
{
public:
	virtual EventType getType() const { return EventType::NONE; }
	virtual std::string eventString() const { return ""; }

	virtual ~Event() {}
};

class EventDispatcher
{
public:
	inline static std::vector<std::unique_ptr<Event>>& GetEventQueue() { return s_queue; }

private:
	static std::vector<std::unique_ptr<Event>> s_queue;
};

// --------------------------------------------------------------------------
// Window
//

class WindowResizeEvent : public Event
{
public:
	int width;
	int height;

	virtual EventType getType() const override { return EventType::WINDOW_RESIZE; }
	virtual std::string eventString() const override;

	WindowResizeEvent(int _width, int _height)
		: width(_width), height(_height) {}
};

class WindowMinimizedEvent : public Event
{
public:
	virtual EventType getType() const override { return EventType::WINDOW_MINIMIZED; }
	virtual std::string eventString() const override;

	WindowMinimizedEvent() {}
};

// --------------------------------------------------------------------------
// Mouse
//

class MouseClickEvent : public Event
{
public:
	int button;
	int mods;

	virtual EventType getType() const override { return EventType::MOUSE_CLICK; }
	virtual std::string eventString() const override;

	MouseClickEvent(int _button, int _mods)
		: button(_button), mods(_mods) {}
};

class MouseReleaseEvent : public Event
{
public:
	int button;
	int mods;

	virtual EventType getType() const override { return EventType::MOUSE_RELEASE; }
	virtual std::string eventString() const override;

	MouseReleaseEvent(int _button, int _mods)
		: button(_button), mods(_mods) {}
};

class MouseMoveEvent : public Event
{
public:
	int xPos;
	int yPos;

	virtual EventType getType() const override { return EventType::MOUSE_MOVE; }
	virtual std::string eventString() const override;

	MouseMoveEvent(int _xPos, int _yPos)
		: xPos(_xPos), yPos(_yPos) {}
};

// --------------------------------------------------------------------------
// Keyboard
//

class KeyPressEvent : public Event
{
public: 
	int key;
	int mods;

	virtual EventType getType() const override { return EventType::KEY_PRESS;  }
	virtual std::string eventString() const override;

	KeyPressEvent(int _key, int _mods) 
		: key(_key), mods(_mods) {}
};

class KeyReleaseEvent : public Event
{
public:
	int key;
	int mods;

	virtual EventType getType() const override { return EventType::KEY_RELEASE;  }
	virtual std::string eventString() const override;

	KeyReleaseEvent(int _key, int _mods)
		: key(_key), mods(_mods) {}
};
