#include "pch.h"
#include "rendering_structures.h"

VkVertexInputBindingDescription Vertex::getBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding   = 0;
	bindingDescription.stride    = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 5> Vertex::getAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};

	attributeDescriptions[0].binding  = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset   = offsetof(Vertex, pos);

	attributeDescriptions[1].binding  = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset   = offsetof(Vertex, color);

	attributeDescriptions[2].binding  = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format   = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[2].offset   = offsetof(Vertex, normal);

	attributeDescriptions[3].binding  = 0;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format   = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[3].offset   = offsetof(Vertex, tangent);

	attributeDescriptions[4].binding  = 0;
	attributeDescriptions[4].location = 4;
	attributeDescriptions[4].format   = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[4].offset   = offsetof(Vertex, texCoord);

	return attributeDescriptions;
}
