#pragma once

#include "Application/logging.h"
#include "device.h"
#include "command.h"

enum class BufferType
{
	NONE    = 0,
	VERTEX  = 1,
	INDEX   = 2,
	UNIFORM = 3
};

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
	static Buffer CreateUniformBuffer(CreateInfo& info);

	const VkBuffer& getBuffer() const;
	const uint32_t getCount() const;
	const VkDeviceSize getSize() const;
	void* getMap();

	void cleanup();

	static void Update(BufferType type, Buffer& buffer, const void* data);

	static void CreateBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory,
		const Device& device);

	static void CopyBuffer(
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

	void* m_map = nullptr;

	// GPU buffer via staging
	Buffer(
		VkBufferUsageFlagBits type,
		const void*           data,
		const VkDeviceSize    dataSize,
		const uint32_t        dataCount,
		const Device&         device,
		const CommandSystem&  commandSystem);

	// Custom buffer
	Buffer(
		const Device&  device,
		VkBuffer       buffer,
		VkDeviceMemory memory,
		VkDeviceSize   size,
		uint32_t       count)
		:
		m_device(&device),
		m_buffer(buffer),
		m_memory(memory),
		m_size  (size),
		m_count (count) 
	{}

	void mapMemory();
};
