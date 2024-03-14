#pragma once

#include <glm/glm.hpp>

struct MeshPushConstants
{
	// Sorted by alignment
	glm::mat4 model       = glm::mat4(1.0f);
	glm::vec3 objectColor = { 0.5f, 0.5f, 0.5f };
	int32_t   objectID    = 0;
};

struct RtxPushConstants
{
	glm::vec4 clearColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	int maxDepth    = 1;
	int sampleCount = 1;
	int frame       = 0;
};

struct GlobalUniform
{
	// Sorted by alignment
	glm::mat4 viewProjection;
	glm::mat4 viewInverse;
	glm::mat4 projInverse;

	glm::vec3 viewPosition;
	float     lightIntensity = 1.0;

	glm::vec3 lightPosition;

	alignas(16) glm::vec3 lightColor;
};

class Vertex
{
public:
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription();

	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();
};
