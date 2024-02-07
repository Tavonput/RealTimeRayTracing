#pragma once

#include "Application/logging.h"
#include "device.h"

struct CommandManagerCreateInfo
{
	const Device* device;

	uint32_t graphicsBufferCount = 1;
};

class CommandManager
{
public:
	void init(CommandManagerCreateInfo& createInfo);

	VkCommandBuffer getCommandBuffer(uint32_t index);

	VkCommandBuffer beginSingleTimeCommands();

	void endSingleTimeCommands(
		VkCommandBuffer commandBuffer,
		const VkQueue& queue);

	void cleanup();

private:
	VkCommandPool                m_pool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_buffers;

	const Device* m_device = nullptr;

	void createCommandPool();
	void createGraphicsBuffers(uint32_t count);
};