#pragma once

#include "Application/logging.h"
#include "device.h"
#include "command.h"

enum class BufferType
{
	NONE = 0,
	VERTEX,
	INDEX,
	UNIFORM,
	STORAGE,
	SCRATCH,
	ACCEL,
	SBT
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
		const char*          name          = "";
		VkBufferUsageFlags   flags         = 0;
	};

	Buffer() {}

	static Buffer CreateVertexBuffer(CreateInfo& info);
	static Buffer CreateIndexBuffer(CreateInfo& info);
	static Buffer CreateUniformBuffer(CreateInfo& info);
	static Buffer CreateStorageBuffer(CreateInfo& info);
	static Buffer CreateScratchBuffer(CreateInfo& info);
	static Buffer CreateAccelerationStructureBuffer(CreateInfo& info);
	static Buffer CreateAccelerationStructureInstanceBuffer(CreateInfo& info);
	static Buffer CreateShaderBindingTableBuffer(CreateInfo& info);

	void map();
	void unmap();

	const VkBuffer& getBuffer() const { return m_buffer; }
	const uint32_t getCount() const { return m_count; }
	const VkDeviceSize getSize() const { return m_size; }
	void* getMap() { return m_map; }
	VkDeviceAddress getDeviceAddress() const;

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
	std::string   m_name = "";

	VkBuffer       m_buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_memory = VK_NULL_HANDLE;

	VkDeviceSize m_size  = 0;
	uint32_t     m_count = 0;

	void* m_map = nullptr;

	// GPU buffer via staging
	Buffer(
		VkBufferUsageFlags    type,
		const void*           data,
		const VkDeviceSize    dataSize,
		const uint32_t        dataCount,
		const Device&         device,
		const CommandSystem&  commandSystem,
		const std::string     name);

	// Custom buffer
	Buffer(
		const Device&     device,
		VkBuffer          buffer,
		VkDeviceMemory    memory,
		VkDeviceSize      size,
		uint32_t          count,
		std::string       name)
		:
		m_device(&device),
		m_buffer(buffer),
		m_memory(memory),
		m_size  (size),
		m_count (count),
		m_name  (name)
	{}
};
