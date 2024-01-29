#pragma once

#include <vulkan/vulkan.h>

#include "Application/logging.h"
#include "device.h"
#include "shader.h"
#include "vertex.h"

class Pipeline
{
public:
	void init(const Device& device, Logger logger, VkRenderPass& renderPass);

	void bind(VkCommandBuffer commandBuffer);

	void cleanup();

private:
	const Device* m_device = nullptr;
	Logger m_logger;

	VkPipeline m_pipeline     = VK_NULL_HANDLE;
	VkPipelineLayout m_layout = VK_NULL_HANDLE;
};