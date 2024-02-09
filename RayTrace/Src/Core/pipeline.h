#pragma once

#include "Application/logging.h"
#include "device.h"
#include "shader.h"
#include "vertex.h"
#include "push_constant.h"

class Pipeline 
{
public:

	enum PipelineType
	{
		MAIN
	};

	// Builder Class
	class Builder
	{
	public:
		Builder(const Device& device);

		Pipeline buildPipeline(const char* vertexShaderPath, const char* fragmentShaderPath, VkRenderPass& renderPass, VkSampleCountFlagBits sampleCount);

	private:
		const Device* m_device;
	};

	// Pipeline Class
	VkPipeline       pipeline = VK_NULL_HANDLE;
	VkPipelineLayout layout   = VK_NULL_HANDLE;

	Pipeline(VkPipeline _pipeline, VkPipelineLayout _layout)
		: pipeline(_pipeline), layout(_layout) {}

	void cleanup(const Device& device);
};
