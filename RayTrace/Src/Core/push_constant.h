#pragma once

#include <glm/glm.hpp>

struct MeshPushConstants
{
	glm::mat4 renderMatrix;
	glm::mat4 model;
};
