#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include "logging.h"
#include "event.h"

class Camera
{
public:
	// Camera creation info
	struct CreateInfo
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };

		float nearPlane   = 0.01f;
		float farPlane    = 100.0f;
		float fov         = 45.0f;
		float sensitivity = 1.0f;

		uint32_t windowWidth  = 0;
		uint32_t windowHeight = 0;
	};

	void init(Camera::CreateInfo& info);

	// Getters
	const glm::mat4 getViewProjection() const { return m_projection * m_view; }
	const glm::vec3 getPosition() const { return m_eye; }
	const float getFov() const { return m_fov; }

	void updatePosition();

	void setWindowSize(uint32_t width, uint32_t height);

	// Callbacks
	void onWindowResize(WindowResizeEvent event);
	void onMouseClick(MouseClickEvent event);
	void onMouseRelease(MouseReleaseEvent event);
	void onMouseMove(MouseMoveEvent event);
	void onKeyPress(KeyPressEvent event);
	void onKeyRelease(KeyReleaseEvent event);

private:
	glm::vec3 m_eye      = { 0.0f, 0.0f, -1.0f };
	glm::vec3 m_up       = { 0.0f, 1.0f,  0.0f };
	glm::vec3 m_center   = { 0.0f, 0.0f,  0.0f };
	glm::vec3 m_worldUp  = { 0.0f, 1.0f,  0.0f };

	glm::mat4 m_view{};
	glm::mat4 m_projection{};

	float m_nearPlane   = 0.01f;
	float m_farPlane    = 100.0f;
	float m_fov         = 45.0f;
	float m_sensitivity = 1.0f;

	uint32_t m_width  = 0;
	uint32_t m_height = 0;

	// Input state
	bool m_leftMouse     = false;
	bool m_rightMouse    = false;
	bool m_shift         = false;
	bool m_wKey          = false;
	bool m_aKey          = false;
	bool m_sKey          = false;
	bool m_dKey          = false;
	glm::vec2 m_mousePos = { 0.0f, 0.0f };

	void updateViewMatrix() { m_view = glm::lookAt(m_eye, m_center, m_up); }

	void orbit(float deltaX, float deltaY);
	void dolly(float deltaX, float deltaY);
};
