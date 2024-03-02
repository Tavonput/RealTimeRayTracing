#include "pch.h"

#include "buffer.h"

Buffer Buffer::CreateVertexBuffer(CreateInfo& info)
{
    return Buffer(
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        info.data,
        info.dataSize,
        info.dataCount,
        *info.device,
        *info.commandSystem,
        info.name);
}

Buffer Buffer::CreateIndexBuffer(CreateInfo& info)
{
    return Buffer(
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        info.data,
        info.dataSize,
        info.dataCount,
        *info.device,
        *info.commandSystem,
        info.name);
}

Buffer Buffer::CreateUniformBuffer(CreateInfo& info)
{
    APP_LOG_INFO("Creating buffer ({})", info.name);

    VkBuffer buffer;
    VkDeviceMemory memory;

    Buffer::CreateBuffer(
        info.dataSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        buffer, memory,
        *info.device);

    // Create custom buffer and map memory
    auto uniformBuffer = Buffer(*info.device, buffer, memory, info.dataSize, 0, info.name);
    uniformBuffer.mapMemory();

    return uniformBuffer;
}

Buffer Buffer::CreateStorageBuffer(CreateInfo& info)
{
    return Buffer(
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        info.data,
        info.dataSize,
        info.dataCount,
        *info.device,
        *info.commandSystem,
        info.name);
}

VkDeviceAddress Buffer::getDeviceAddress() const
{
    VkBufferDeviceAddressInfo info{};
    info.sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    info.buffer = m_buffer;

    return vkGetBufferDeviceAddress(m_device->getLogical(), &info);
}

void Buffer::cleanup()
{
    APP_LOG_INFO("Destroying buffer ({})", m_name);

    vkDestroyBuffer(m_device->getLogical(), m_buffer, nullptr);
    vkFreeMemory(m_device->getLogical(), m_memory, nullptr);
}

void Buffer::Update(BufferType type, Buffer& buffer, const void* data)
{
    switch (type)
    {
        case BufferType::UNIFORM:
            memcpy(buffer.getMap(), data, buffer.getSize());
            break;
    }
}

void Buffer::CreateBuffer(
    VkDeviceSize          size, 
    VkBufferUsageFlags    usage, 
    VkMemoryPropertyFlags properties, 
    VkBuffer&             buffer, 
    VkDeviceMemory&       bufferMemory, 
    const Device&         device)
{
    // Create buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = size;
    bufferInfo.usage       = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device.getLogical(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        APP_LOG_CRITICAL("Failed to create buffer");
        throw;
    }

    // Get memory requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.getLogical(), buffer, &memRequirements);

    // Memory allocation flags
    VkMemoryAllocateFlagsInfo allocFlags{};
    allocFlags.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    allocFlags.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

    // Memory allocation information
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext           = &allocFlags;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = Device::findMemoryType(memRequirements.memoryTypeBits, properties, device.getPhysical());

    if (vkAllocateMemory(device.getLogical(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        APP_LOG_CRITICAL("Failed to allocate buffer memory");
        throw;
    }

    // Bind memory
    vkBindBufferMemory(device.getLogical(), buffer, bufferMemory, 0);
}

void Buffer::CopyBuffer(
    VkBuffer srcBuffer, 
    VkBuffer dstBuffer, 
    VkDeviceSize size, 
    const CommandSystem& commandSystem, 
    const VkQueue& queue)
{
    // Allocate and begin a single-use command buffer
    VkCommandBuffer commandBuffer = commandSystem.beginSingleTimeCommands();

    // Copy data from src to dst buffers
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size      = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    // End and submit single-use command buffer
    commandSystem.endSingleTimeCommands(commandBuffer, queue);
}

Buffer::Buffer(
    VkBufferUsageFlags type,
    const void* data,
    const VkDeviceSize dataSize,
    const uint32_t dataCount,
    const Device& device,
    const CommandSystem& commandPool,
    const std::string name)
{
    m_device = &device;
    m_name = name;

    m_size = dataSize;
    m_count = dataCount;

    APP_LOG_INFO("Creating buffer ({})", name);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    // Create staging buffer
    Buffer::CreateBuffer(
        dataSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingMemory,
        *m_device);

    // Transfer buffer data into staging buffer memory
    void* deviceData;
    vkMapMemory(m_device->getLogical(), stagingMemory, 0, dataSize, 0, &deviceData);
    memcpy(deviceData, data, (size_t)dataSize);
    vkUnmapMemory(m_device->getLogical(), stagingMemory);

    // Create buffer
    Buffer::CreateBuffer(
        dataSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | type,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_buffer, m_memory,
        *m_device);

    // Copy staging buffer to device buffer
    Buffer::CopyBuffer(
        stagingBuffer, m_buffer,
        dataSize,
        commandPool, m_device->getGraphicsQueue());

    vkDestroyBuffer(m_device->getLogical(), stagingBuffer, nullptr);
    vkFreeMemory(m_device->getLogical(), stagingMemory, nullptr);
}

void Buffer::mapMemory()
{
    vkMapMemory(m_device->getLogical(), m_memory, 0, m_size, 0, &m_map);
}
