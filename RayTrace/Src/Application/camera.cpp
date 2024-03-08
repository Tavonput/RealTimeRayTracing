#include "pch.h"
#include "camera.h"

void Camera::init(Camera::CreateInfo& info)
{
	m_eye         = info.position;
	m_nearPlane   = info.nearPlane;
	m_farPlane    = info.farPlane;
	m_fov         = info.fov;
	m_sensitivity = info.sensitivity;

	m_width  = info.windowWidth;
	m_height = info.windowHeight;

	// Set view matrix
	updateViewMatrix();

	// Set projection matrix
	m_projection = glm::perspective(m_fov, m_width / (float)m_height, m_nearPlane, m_farPlane);
	m_projection[1][1] *= -1;
}

void Camera::setWindowSize(uint32_t width, uint32_t height)
{
	m_width  = width;
	m_height = height;
}

void Camera::onWindowResize(WindowResizeEvent event)
{
	m_width  = event.width;
	m_height = event.height;

	m_projection = glm::perspective(m_fov, m_width / (float)m_height, m_nearPlane, m_farPlane);
	m_projection[1][1] *= -1;
}

void Camera::onMouseClick(MouseClickEvent event)
{
	switch (event.button)
	{
		case GLFW_MOUSE_BUTTON_LEFT:  m_leftMouse  = true; break;
		case GLFW_MOUSE_BUTTON_RIGHT: m_rightMouse = true; break;
	}
}

void Camera::onMouseRelease(MouseReleaseEvent event)
{
	switch (event.button)
	{
		case GLFW_MOUSE_BUTTON_LEFT:  m_leftMouse  = false; break;
		case GLFW_MOUSE_BUTTON_RIGHT: m_rightMouse = false; break;
	}
}

void Camera::onMouseMove(MouseMoveEvent event)
{
	if (!m_leftMouse && !m_rightMouse)
	{
		// No action, just update the mouse position
		m_mousePos.x = static_cast<float>(event.xPos);
		m_mousePos.y = static_cast<float>(event.yPos);
		return;
	}

	// Compute mouse position offset
	float deltaX = float(event.xPos - m_mousePos.x) / float(m_width);
	float deltaY = float(event.yPos - m_mousePos.y) / float(m_height);

	// Update camera
	if (m_leftMouse)
		orbit(deltaX, deltaY);

	else if (m_rightMouse)
		dolly(deltaX, deltaY);

	updateViewMatrix();

	// Update mouse position
	m_mousePos.x = static_cast<float>(event.xPos);
	m_mousePos.y = static_cast<float>(event.yPos);
}

void Camera::updatePosition() { // Updates camera position according to 
	if (m_wKey) APP_LOG_INFO("W Key Down");
	if (m_aKey) APP_LOG_INFO("A Key Down");
	if (m_sKey) APP_LOG_INFO("S Key Down");
	if (m_dKey) APP_LOG_INFO("D Key Down");


	
}

void Camera::resetPosition()
{
	m_eye = { 0.0f, 0.0f, 0.0f };
}

void Camera::flyMode()
{
	m_cameraMode = CameraMode::FLY;
}

void Camera::onKeyPress(KeyPressEvent event)
{
	if      (event.key == GLFW_KEY_W) m_wKey = true;
	else if (event.key == GLFW_KEY_A) m_aKey = true;
	else if (event.key == GLFW_KEY_S) m_sKey = true;
	else if (event.key == GLFW_KEY_D) m_dKey = true;
}

void Camera::onKeyRelease(KeyReleaseEvent event)
{
	if      (event.key == GLFW_KEY_W) m_wKey = false;
	else if (event.key == GLFW_KEY_A) m_aKey = false;
	else if (event.key == GLFW_KEY_S) m_sKey = false;
	else if (event.key == GLFW_KEY_D) m_dKey = false;
}


void Camera::orbit(float deltaX, float deltaY)
{
	// Find the scaling factor for rotation speed
	deltaX *= glm::two_pi<float>() * m_sensitivity;
	deltaY *= glm::two_pi<float>() * m_sensitivity;

	// Find center to eye, this will be the camera's z-axis
	glm::vec3 centerToEye(m_eye - m_center);
	float radius    = glm::length(centerToEye);
	centerToEye     = glm::normalize(centerToEye);
	glm::vec3 axisZ = centerToEye;

	// Rotate around the y-axis
	glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), -deltaX, m_up);
	centerToEye = rotationY * glm::vec4(centerToEye, 0.0f);

	// Find the camera's x-axis
	glm::vec3 axisX = glm::normalize(glm::cross(m_up, axisZ));

	// Rotate around the x-axis
	glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), -deltaY, axisX);
	glm::vec3 rotationXVec = rotationX * glm::vec4(centerToEye, 0.0f);

	// Check if the rotation is within the same hemisphere to avoid the camera flipping over
	if (glm::sign(rotationXVec.x) == glm::sign(centerToEye.x) && glm::sign(rotationXVec.z) == glm::sign(centerToEye.z))
		centerToEye = rotationXVec;

	centerToEye *= radius;
	m_eye = centerToEye + m_center;
}

void Camera::dolly(float deltaX, float deltaY)
{
	glm::vec3 centerToEye = m_center - m_eye;
	float length = static_cast<float>(glm::length(centerToEye));

	// Don't go past the center
	if (length < 0.00001f)
		return;

	// Choose the delta with the larger magnitude
	float delta = fabs(deltaX) > fabs(deltaY) ? deltaX : -deltaY;

	m_eye += centerToEye * delta;
}
