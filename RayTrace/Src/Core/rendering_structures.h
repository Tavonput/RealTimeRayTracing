#pragma once

#include <glm/glm.hpp>

struct MeshPushConstants
{
	// Initialize as identity matrix
	glm::mat4 model = 
		{ 1.0f, 0.0f, 0.0f, 0.0f, 
		  0.0f, 1.0f, 0.0f, 0.0f, 
		  0.0f, 0.0f, 1.0f, 0.0f, 
		  0.0f, 0.0f, 0.0f, 1.0f };

	glm::vec3 objectColor;
};

struct GlobalUniform
{
	glm::mat4 viewProjection;

	alignas(16) glm::vec3 lightPosition;
	alignas(16) glm::vec3 lightColor;
	alignas(16) glm::vec3 viewPosition;
};

class Vertex
{
public:
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 normal;

	static VkVertexInputBindingDescription getBindingDescription();

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};