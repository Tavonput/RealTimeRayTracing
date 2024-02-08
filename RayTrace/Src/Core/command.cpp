#include "pch.h"

#include "command.h"

void CommandSystem::init(const Device& device, uint32_t bufferCount)
{
	m_device = &device;

	m_buffers.resize(bufferCount);

	createCommandPool();
    createGraphicsBuffers(bufferCount);
}

VkCommandBuffer CommandSystem::getCommandBuffer(uint32_t index)
{
    return m_buffers[index];
}

VkCommandBuffer CommandSystem::beginSingleTimeCommands()
{
    // Allocate a single use command buffer
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = m_pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_device->getLogical(), &allocInfo, &commandBuffer);

    // Begin the command buffer
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void CommandSystem::endSingleTimeCommands(VkCommandBuffer commandBuffer, const VkQueue& queue)
{
    // End command buffer
    vkEndCommandBuffer(commandBuffer);

    // Submit command
    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    // Free command buffer
    vkFreeCommandBuffers(m_device->getLogical(), m_pool, 1, &commandBuffer);
}

void CommandSystem::cleanup()
{
    APP_LOG_INFO("Destroying command manager");

    vkDestroyCommandPool(m_device->getLogical(), m_pool, nullptr);
}

void CommandSystem::createCommandPool()
{
    APP_LOG_INFO("Initializing command pool");

    // Create a command pool for the graphics family
    QueueFamilyIndices queueFamilyIndices = m_device->getIndicies();
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(m_device->getLogical(), &poolInfo, nullptr, &m_pool) != VK_SUCCESS)
    {
        APP_LOG_CRITICAL("Failed to create command pool");
        throw;
    }

    APP_LOG_INFO("Command pool initialization successful");
}

void CommandSystem::createGraphicsBuffers(uint32_t count)
{
    APP_LOG_INFO("Allocating {} command buffers for graphics rendering", count);

    // Allocate command buffers for the main render loop
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = m_pool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    if (vkAllocateCommandBuffers(m_device->getLogical(), &allocInfo, m_buffers.data()) != VK_SUCCESS)
    {
        APP_LOG_CRITICAL("Failed to allocate command buffers");
        throw;
    }

    APP_LOG_INFO("Command buffer allocation successful");
}
