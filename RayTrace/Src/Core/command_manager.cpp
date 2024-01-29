#include "command_manager.h"

void CommandManager::init(CommandManagerCreateInfo& createInfo)
{
	m_device = createInfo.device;
	m_logger = createInfo.logger;

	m_buffers.resize(createInfo.graphicsBufferCount);

	createCommandPool();
    createGraphicsBuffers(createInfo.graphicsBufferCount);
}

VkCommandBuffer CommandManager::beginSingleTimeCommands()
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

void CommandManager::endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue& queue)
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

void CommandManager::cleanup()
{
    LOG_INFO("Destroying command manager");

    vkDestroyCommandPool(m_device->getLogical(), m_pool, nullptr);
}

void CommandManager::createCommandPool()
{
    LOG_INFO("Initializing command pool");

    // Create a command pool for the graphics family
    QueueFamilyIndices queueFamilyIndices = m_device->getIndicies();
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(m_device->getLogical(), &poolInfo, nullptr, &m_pool) != VK_SUCCESS)
    {
        LOG_CRITICAL("Failed to create command pool");
        throw;
    }

    LOG_INFO("Command pool initialization successful");
}

void CommandManager::createGraphicsBuffers(uint32_t count)
{
    LOG_INFO("Allocating {} command buffers for graphics rendering", count);

    // Allocate command buffers for the main render loop
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = m_pool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    if (vkAllocateCommandBuffers(m_device->getLogical(), &allocInfo, m_buffers.data()) != VK_SUCCESS)
    {
        LOG_CRITICAL("Failed to allocate command buffers");
        throw;
    }

    LOG_INFO("Command buffer allocation successful");
}
