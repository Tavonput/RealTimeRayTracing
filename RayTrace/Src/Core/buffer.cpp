#include "buffer.h"

Buffer::Buffer(
    BufferType type, 
    const void* data, 
    const VkDeviceSize dataSize,
    const uint32_t dataCount,
    const Device& device, 
    CommandManager& commandPool, 
    Logger logger)
{
    m_device = &device;
    m_logger = logger;

    m_size  = dataSize;
    m_count = dataCount;

    LOG_INFO("Creating buffer");

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    // Create staging buffer
    Buffer::createBuffer(
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
    Buffer::createBuffer(
        dataSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | static_cast<VkBufferUsageFlags>(type),
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_buffer, m_memory,
        *m_device);

    // Copy staging buffer to device buffer
    Buffer::copyBuffer(
        stagingBuffer, m_buffer,
        dataSize,
        commandPool, m_device->getGraphicsQueue());

    vkDestroyBuffer(m_device->getLogical(), stagingBuffer, nullptr);
    vkFreeMemory(m_device->getLogical(), stagingMemory, nullptr);
}

const VkBuffer& Buffer::getBuffer() const
{
    return m_buffer;
}

const uint32_t Buffer::getCount() const
{
    return m_count;
}

void Buffer::cleanup()
{
    LOG_INFO("Destroying buffer");

    vkDestroyBuffer(m_device->getLogical(), m_buffer, nullptr);
    vkFreeMemory(m_device->getLogical(), m_memory, nullptr);
}

void Buffer::createBuffer(
    VkDeviceSize size, 
    VkBufferUsageFlags usage, 
    VkMemoryPropertyFlags properties, 
    VkBuffer& buffer, 
    VkDeviceMemory& bufferMemory, 
    const Device& device)
{
    // Create buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = size;
    bufferInfo.usage       = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device.getLogical(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to create buffer");

    // Get memory requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.getLogical(), buffer, &memRequirements);

    // Memory allocation information
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Device::findMemoryType(memRequirements.memoryTypeBits, properties, device.getPhysical());

    if (vkAllocateMemory(device.getLogical(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate vertex buffer memory!");

    // Bind memory
    vkBindBufferMemory(device.getLogical(), buffer, bufferMemory, 0);
}

void Buffer::copyBuffer(
    VkBuffer srcBuffer, 
    VkBuffer dstBuffer, 
    VkDeviceSize size, 
    CommandManager& commandManager, 
    const VkQueue& queue)
{
    // Allocate and begin a single-use command buffer
    VkCommandBuffer commandBuffer = commandManager.beginSingleTimeCommands();

    // Copy data from src to dst buffers
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size      = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    // End and submit single-use command buffer
    commandManager.endSingleTimeCommands(commandBuffer, queue);
}
