#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "logging.h"

class Camera
{
public:
	// Camera creation info
	struct CreateInfo
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };

		float nearPlane    = 0.01f;
		float farPlane     = 100.0f;
		float fov          = 45.0f;
	};

	void init(Camera::CreateInfo& info);

	const glm::mat4 getViewProjection(float aspectRatio) const;
	const glm::vec3 getPosition() const;
	const float getFov() const;

private:
	glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };

	float m_nearPlane    = 0.01f;
	float m_farPlane     = 100.0f;
	float m_fov          = 45.0f;
};
