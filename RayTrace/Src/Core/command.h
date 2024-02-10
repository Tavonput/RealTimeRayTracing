#pragma once

#include "Application/logging.h"
#include "device.h"

class CommandSystem
{
public:
	void init(const Device& device, uint32_t bufferCount);

	VkCommandBuffer getCommandBuffer(uint32_t index);

	VkCommandBuffer beginSingleTimeCommands() const;

	void endSingleTimeCommands(VkCommandBuffer commandBuffer, const VkQueue& queue) const;

	void cleanup();

private:
	VkCommandPool                m_pool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_buffers;

	const Device* m_device = nullptr;

	void createCommandPool();
	void createGraphicsBuffers(uint32_t count);
};