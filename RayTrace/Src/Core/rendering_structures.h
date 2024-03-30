#pragma once

#include <glm/glm.hpp>

struct MeshPushConstants
{
	// Sorted by alignment
	glm::mat4 model       = glm::mat4(1.0f);
	glm::vec3 objectColor = { 0.5f, 0.5f, 0.5f };
	int32_t   objectID    = 0;
};

struct PostPushConstants
{
	float exposure = 1.0f;
};

struct RtxPushConstants
{
	glm::vec4 clearColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	int maxDepth        = 1;
	int sampleCount     = 1;
	int frame           = 0;

	float russianRoulette = 1.0f;
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
	int       debugMode;

	glm::vec3 lightColor;
};

class Vertex
{
public:
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription();

	static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions();
};
