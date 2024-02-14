#include "pch.h"
#include "camera.h"

void Camera::init(Camera::CreateInfo& info)
{
	m_position = info.position;
	m_nearPlane = info.nearPlane;
	m_farPlane = info.farPlane;
	m_fov = info.fov;
}

const glm::mat4 Camera::getViewProjection(float aspectRatio) const
{
	glm::mat4 view = glm::lookAt(m_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(m_fov, aspectRatio, m_nearPlane, m_farPlane);
	projection[1][1] *= -1;

	return projection * view;
}

const glm::vec3 Camera::getPosition() const
{
	return m_position;
}

const float Camera::getFov() const
{
	return m_fov;
}
