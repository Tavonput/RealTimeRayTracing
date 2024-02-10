#pragma once

#include "Application/logging.h"
#include "device.h"
#include "command.h"

class Buffer
{
public:
	struct CreateInfo
	{
		const void*          data          = nullptr;
		VkDeviceSize         dataSize      = 0;
		uint32_t             dataCount     = 0;
		const Device*        device        = nullptr;
		const CommandSystem* commandSystem = nullptr;
	};

	Buffer() {}

	static Buffer CreateVertexBuffer(CreateInfo& info);
	static Buffer CreateIndexBuffer(CreateInfo& info);

	const VkBuffer& getBuffer() const;
	const uint32_t getCount() const;

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
		const CommandSystem& commandSystem,
		const VkQueue& queue);

private:
	const Device* m_device = nullptr;

	VkBuffer       m_buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_memory = VK_NULL_HANDLE;

	VkDeviceSize m_size  = 0;
	uint32_t     m_count = 0;

	Buffer(
		VkBufferUsageFlagBits type,
		const void* data,
		const VkDeviceSize dataSize,
		const uint32_t dataCount,
		const Device& device,
		const CommandSystem& commandSystem);
};
