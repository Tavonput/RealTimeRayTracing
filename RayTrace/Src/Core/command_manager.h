#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Application/logging.h"
#include "device.h"

struct CommandManagerCreateInfo
{
	const Device* device;
	Logger        logger;

	uint32_t graphicsBufferCount = 1;
};

class CommandManager
{
public:
	void init(CommandManagerCreateInfo& createInfo);

	VkCommandBuffer beginSingleTimeCommands();

	void endSingleTimeCommands(
		VkCommandBuffer commandBuffer,
		VkQueue& queue);

	void cleanup();

private:
	VkCommandPool                m_pool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_buffers;

	const Device* m_device = nullptr;

	Logger  m_logger;

	void createCommandPool();
	void createGraphicsBuffers(uint32_t count);
};