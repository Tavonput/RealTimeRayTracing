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

	m_window = info.window;

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
	if (!m_leftMouse && !m_rightMouse && m_cameraMode == CameraMode::STATIONARY)
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
	if (m_leftMouse && m_cameraMode == CameraMode::STATIONARY)
		orbit(deltaX, deltaY);

	else if (m_rightMouse && m_cameraMode == CameraMode::STATIONARY)
		dolly(deltaX, deltaY);

	else if (m_cameraMode == CameraMode::FPV)
		//updateDirection(deltaX, deltaY);
		orbit(deltaX, deltaY);

	updateViewMatrix();

	// Update mouse position
	m_mousePos.x = static_cast<float>(event.xPos);
	m_mousePos.y = static_cast<float>(event.yPos);
}

void Camera::resetPosition()
{
	m_eye = { 0.0f, 0.0f, 6.0f };
	m_center = { 0.0f, 0.0f, 0.0f };
	m_up = { 0.0f, 1.0f, 0.0f };
}

void Camera::onKeyPress(KeyPressEvent event)
{
	switch (event.key)
	{
		case GLFW_KEY_W: m_wKey = true; break;
		case GLFW_KEY_A: m_aKey = true; break;
		case GLFW_KEY_S: m_sKey = true; break;
		case GLFW_KEY_D: m_dKey = true; break;
		case GLFW_KEY_LEFT_SHIFT: m_lShift = true; break;
		case GLFW_KEY_SPACE: m_space = true; break;
	}
}

void Camera::onKeyRelease(KeyReleaseEvent event)
{
	switch (event.key)
	{
		case GLFW_KEY_W: m_wKey = false; break;
		case GLFW_KEY_A: m_aKey = false; break;
		case GLFW_KEY_S: m_sKey = false; break;
		case GLFW_KEY_D: m_dKey = false; break;
		case GLFW_KEY_LEFT_SHIFT: m_lShift = false; break;
		case GLFW_KEY_SPACE: m_space = false; break;
		case GLFW_KEY_ESCAPE: // Will likely change keybinds later on. 
			// Switches between fly mode and stationary mode. 
			if (m_cameraMode == CameraMode::STATIONARY) FPVMode();
			else if (m_cameraMode == CameraMode::FPV) stationaryMode();
			break;
	}
}

void Camera::FPVMode()
{
	glfwSetInputMode(m_window.getWindowGLFW(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	m_cameraMode = CameraMode::FPV;
}

void Camera::stationaryMode()
{
	glfwSetInputMode(m_window.getWindowGLFW(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	m_cameraMode = CameraMode::STATIONARY;
	resetPosition();
	updateViewMatrix();
	m_firstMouseMove = true;
}

void Camera::updatePosition() {

	if (m_cameraMode == CameraMode::STATIONARY) return;

	m_currentFrame = glfwGetTime(); // Computes the change in time
	float deltaT = m_currentFrame - m_lastFrame;
	m_lastFrame = m_currentFrame;

	glm::vec3 axisZ = glm::normalize(m_eye - m_center); // Normalized Z-axis relative to camera (Direction camera is looking)
	glm::vec3 axisX = glm::normalize(glm::cross(m_up, axisZ)); // Normalized X-axis relative to camera

	const float cameraSpeed = 3.0f * deltaT;

	if (m_wKey) {
		//APP_LOG_INFO("W Key Down");
		m_eye -= cameraSpeed * axisZ; // Moves camera position forwards (Z-axis represents forward and backwards for camera)
	}
	if (m_aKey) {
		//APP_LOG_INFO("A Key Down");
		m_eye -= cameraSpeed * axisX; // Moves camera to the left (X-axis represents left and right of camera)
		m_center -= cameraSpeed * axisX; // Moves point that camera is looking at to the left
	}
	if (m_sKey) {
		//APP_LOG_INFO("S Key Down");
		m_eye += cameraSpeed * axisZ; // Moves camera position backwards (Z-axis represents forward and backwards of camera)
	}
	if (m_dKey) {
		//APP_LOG_INFO("D Key Down");
		m_eye += cameraSpeed * axisX; // Moves camera to the right (X-axis represents left and right of camera)
		m_center += cameraSpeed * axisX; // Moves point that camera is looking at to the right
	}
	if (m_lShift) {
		//APP_LOG_INFO("Left-Shift Key Down");
		m_eye -= cameraSpeed * m_worldUp; // Moves camera down
		m_center -= cameraSpeed * m_worldUp;
	}
	if (m_space) {
		//APP_LOG_INFO("Space Key Down"); 
		m_eye += cameraSpeed * m_worldUp; // Moves camera up
		m_center += cameraSpeed * m_worldUp;
	}
	updateViewMatrix();


}

void Camera::updateDirection(float deltaX, float deltaY)
{
	float sensitivity = 5.0f;

	if (m_firstMouseMove) {

	}
	
	deltaX *= sensitivity;
	deltaY *= sensitivity * -1;

	m_yaw += deltaX;
	m_pitch += deltaY;

	if (m_pitch > 89.0f)
		m_pitch = 89.0f; // Stops camera from going too far up

	if (m_pitch < -89.0f)
		m_pitch = -89.0f; // Stops camera from going too far down

	glm::vec3 direction;
	direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	direction.y = sin(glm::radians(m_pitch));
	direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_center = m_eye + glm::normalize(direction);
	
	glm::vec3 axisZ = glm::normalize(m_eye - m_center);
	glm::vec3 axisX = glm::normalize(glm::cross(m_up, axisZ));
	//m_up = glm::normalize(glm::cross(axisZ, axisX));
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

	if (m_cameraMode == CameraMode::STATIONARY) 
	{
		centerToEye *= radius;
		m_eye = centerToEye + m_center;
	}
	else if (m_cameraMode == CameraMode::FPV)
	{
		m_center = m_eye + centerToEye;
	}
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
