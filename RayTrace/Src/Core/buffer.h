#pragma once

#include <vulkan/vulkan.h>

#include "Application/logging.h"
#include "device.h"
#include "command_manager.h"

enum class BufferType
{
	VERTEX = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	INDEX  = VK_BUFFER_USAGE_INDEX_BUFFER_BIT
};

class Buffer
{
public:
	Buffer() {}

	Buffer(
		BufferType type,
		const void* data,
		const VkDeviceSize dataSize,
		const Device& device,
		CommandManager& commandPool,
		Logger logger);

	const VkBuffer& getBuffer() const;

	void cleanup();

	static void createBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory,
		const Device& device);

	static void copyBuffer(
		VkBuffer srcBuffer,
		VkBuffer dstBuffer,
		VkDeviceSize size,
		CommandManager& commandManager,
		const VkQueue& queue);

private:
	const Device* m_device = nullptr;

	Logger m_logger;

	VkBuffer       m_buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_memory = VK_NULL_HANDLE;
};