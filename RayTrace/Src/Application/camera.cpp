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
	m_projection = glm::perspective(glm::radians(m_fov), m_width / (float)m_height, m_nearPlane, m_farPlane);
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

	m_projection = glm::perspective(glm::radians(m_fov), m_width / (float)m_height, m_nearPlane, m_farPlane);
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
	if (!m_leftMouse && !m_rightMouse && m_cameraMode == CameraMode::STATIONARY || m_mousePause)
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

	else if (m_cameraMode != CameraMode::STATIONARY)
		updateDirection(deltaX, deltaY);
		//orbit(deltaX, deltaY);

	updateViewMatrix();

	// Update mouse position
	m_mousePos.x = static_cast<float>(event.xPos);
	m_mousePos.y = static_cast<float>(event.yPos);
}

void Camera::pauseMouseMove()
{
	if (m_cameraMode == CameraMode::STATIONARY) return;
	if (m_mousePause)
	{
		glfwSetInputMode(m_window.getWindowGLFW(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_mousePause = false;
	}
	else
	{
		glfwSetInputMode(m_window.getWindowGLFW(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_mousePause = true;
	}
}

void Camera::resetPosition()
{
	m_eye = { 0.0f, 1.0f, 6.0f };
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
		case GLFW_KEY_SPACE: spacePress(); break;
		case GLFW_KEY_LEFT_CONTROL: m_lCtrl = true; break;
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
		case GLFW_KEY_LEFT_CONTROL: m_lCtrl = false; break;
		case GLFW_KEY_ESCAPE:  pauseMouseMove(); break;
	}
}

void Camera::spacePress() // Computes time between space presses. Used for toggling flying
{
	m_space = true;
	float currentTime = static_cast<float>(glfwGetTime());
	float deltaT = currentTime - m_lastSpacePressTime;
	if (deltaT <= 0.25f)
		m_fly = !m_fly;
	m_lastSpacePressTime = currentTime;
}

void Camera::creativeMode()
{
	m_cameraMode = CameraMode::CREATIVE;
	glfwSetInputMode(m_window.getWindowGLFW(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	m_fly = false;
	m_lastSpacePressTime = 0;
}

void Camera::FPVMode()
{
	m_mousePause = false;
	glfwSetInputMode(m_window.getWindowGLFW(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	m_cameraMode = CameraMode::FPV;
}

void Camera::stationaryMode()
{
	m_mousePause = false;
	glfwSetInputMode(m_window.getWindowGLFW(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	m_cameraMode = CameraMode::STATIONARY;
	resetPosition();
	updateViewMatrix();
}

void Camera::updateMode(int mode)
{
	if (mode == 0 && m_cameraMode != CameraMode::STATIONARY) Camera::stationaryMode();
	else if (mode == 1 && m_cameraMode != CameraMode::FPV) Camera::FPVMode();
	else if (mode == 2 && m_cameraMode != CameraMode::CREATIVE) Camera::creativeMode();
}

void Camera::updateFov(float newFov)
{
	m_fov = newFov;

	m_projection = glm::perspective(glm::radians(m_fov), m_width / (float)m_height, m_nearPlane, m_farPlane);
	m_projection[1][1] *= -1;
}

void Camera::saveCamera(int cameraSaves)
{
	if (cameraSaves == m_cameraSaves || m_cameraMode == CameraMode::STATIONARY) return;
	
	m_eyePositions.push_back(m_eye);
	m_centerPositions.push_back(m_center);

	m_cameraSaves++;
}

void Camera::switchCameras(int currentCamera)
{
	if (m_cameraSaves == 0 || m_cameraMode == CameraMode::STATIONARY) return;

	if (currentCamera < 0) currentCamera *= -1;

	int cameraIndex = currentCamera % m_cameraSaves;
	if (cameraIndex == m_currentIndex) return;

	m_eye = m_eyePositions[cameraIndex];
	m_center = m_centerPositions[cameraIndex];
	m_currentIndex = cameraIndex;

}

void Camera::updatePosition(float deltaT) {

	if (m_cameraMode == CameraMode::STATIONARY) return;
	
	glm::vec3 axisZ = glm::normalize(m_eye - m_center); // Normalized Z-axis relative to camera (Direction camera is looking)
	glm::vec3 axisX = glm::normalize(glm::cross(m_up, axisZ)); // Normalized X-axis relative to camera

	float cameraSpeed = m_speed * deltaT;
	float yCenter = m_center[1]; 

	if (m_cameraMode == CameraMode::CREATIVE)
	{
		axisZ[1] = 0; // Changes z-axis so the z-direction is relative to world instead of camera
		axisZ = glm::normalize(axisZ);
		m_velocity[0] = 0;
		m_velocity[2] = 0;
		if (!m_fly) m_velocity[1] -= m_gravity * deltaT; // Accelerates the camera downwards with gravity
		else m_velocity[1] = 0;
	}
	else m_velocity = glm::vec3(0); // Resets velocity vector

	if (m_lCtrl) { // Sprint
		cameraSpeed *= 3;
	}	

	if (m_wKey) {
		//APP_LOG_INFO("W Key Down");
		m_velocity -= cameraSpeed * axisZ;
	}
	if (m_aKey) {
		//APP_LOG_INFO("A Key Down");
		m_velocity -= cameraSpeed * axisX;
	}
	if (m_sKey) {
		//APP_LOG_INFO("S Key Down");
		m_velocity += cameraSpeed * axisZ;
	}
	if (m_dKey) {
		//APP_LOG_INFO("D Key Down");
		m_velocity += cameraSpeed * axisX;
	}

	cameraSpeed = m_speed * deltaT; // Sets cameraSpeed back to default so sprinting doesn't change vertical speed. 
	if (m_lShift && m_cameraMode == CameraMode::FPV) {
		//APP_LOG_INFO("Left-Shift Key Down");
		m_velocity -= cameraSpeed * m_worldUp;
	}
	else if (m_lShift && m_fly) { // Creative mode
		m_velocity -= cameraSpeed * m_worldUp;
	}
	if (m_space && m_cameraMode == CameraMode::FPV) {
		//APP_LOG_INFO("Space Key Down"); 
		m_velocity += cameraSpeed * m_worldUp;
	}
	else if (m_space && m_fly) { // Creative mode
		m_velocity += cameraSpeed * m_worldUp;
	}

	if (m_cameraMode == CameraMode::CREATIVE && m_eye[1] <= m_ground )
	{
		m_eye[1] = m_ground;
		m_center[1] = yCenter;
		if (m_space) m_velocity[1] = m_jumpSpeed;
		else m_velocity[1] = 0;
	}
	m_eye += m_velocity;
	m_center += m_velocity;
	updateViewMatrix();
}

void Camera::updateDirection(float deltaX, float deltaY)
{

	deltaX *= glm::pi<float>() * m_sensitivity;
	deltaY *= glm::pi<float>() * m_sensitivity;

	// Calculates vector from camera's position to its center coordinate. Represents camera's z-axis
	glm::vec3 eyeToCenter(m_center - m_eye);
	float length = glm::length(eyeToCenter);
	eyeToCenter = glm::normalize(eyeToCenter);

	//Gets the matrix to rotate vector around y-coordinate
	glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), -deltaX, m_up);

	// Applies the rotation transformation to eyeToCenter (z-axis)
	eyeToCenter = rotationY * glm::vec4(eyeToCenter, 0.0f); 
	
	// Calculates the cross product between the up vector (y-axis) with eyeToCenter (z-axis) to get x-axis
	glm::vec3 axisX = glm::normalize(glm::cross(m_up, eyeToCenter));

	// Gets the matrix to rotate vector around x-coordinate
	glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), deltaY, axisX);

	// Applies the rotation transformation to eyeToCenter. Stores in temporary variable
	glm::vec3 rotationXVec = rotationX * glm::vec4(eyeToCenter, 0.0f);

	// If rotation around x-axis goes beyond +- 90 degrees, rotation is not applied. Prevents camera from flipping over
	if (glm::sign(rotationXVec.x) == glm::sign(eyeToCenter.x) && glm::sign(rotationXVec.z) == glm::sign(eyeToCenter.z))
		eyeToCenter = rotationXVec;

	m_center = m_eye + eyeToCenter * length;
}

void Camera::updateGround(float ground)
{
	if (m_cameraMode == CameraMode::CREATIVE && m_eye[1] <= m_ground)
	{
		m_eye[1] = ground; // Sets the eye's y-coordinate to be ground height
		m_center[1] = m_center[1] + (ground - m_ground); // Adjusts camera center by change in ground height
	}
	m_ground = ground;
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
