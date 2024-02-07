#pragma once

#include "Application/logging.h"
#include "device.h"
#include "shader.h"
#include "vertex.h"

class Pipeline 
{
public:

	// Builder Class
	class Builder
	{
	public:
		Builder(const Device& device);

		Pipeline buildPipeline(const char* vertexShaderPath, const char* fragmentShaderPath, VkRenderPass& renderPass, VkSampleCountFlagBits sampleCount);

	private:
		const Device* m_device;
	};

	// Manager Class
	class Manager
	{
	public:
		void init(const Device& device);

		void addPipeline(Pipeline pipeline);
		void bindPipeline(uint32_t index, VkCommandBuffer commandBuffer);

		void cleanup();

	private:
		const Device* m_device;

		std::vector<Pipeline> m_pipelines;
	};

	// Pipeline Class
	VkPipeline       pipeline = VK_NULL_HANDLE;
	VkPipelineLayout layout   = VK_NULL_HANDLE;

	Pipeline(VkPipeline _pipeline, VkPipelineLayout _layout)
		: pipeline(_pipeline), layout(_layout) {}
};
