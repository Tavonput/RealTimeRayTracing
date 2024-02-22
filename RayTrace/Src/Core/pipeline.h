#pragma once

#include "Application/logging.h"
#include "device.h"
#include "shader.h"
#include "rendering_structures.h"
#include "render_pass.h"
#include "descriptor.h"

class Pipeline 
{
public:

	enum PipelineType
	{
		LIGHTING,
		FLAT
	};

	// Builder Class
	class Builder
	{
	public:
		Builder(const Device& device);

		Pipeline buildPipeline();
		void reset();

		void addGraphicsBase();

		void linkRenderPass(RenderPass& pass);
		void linkShaders(RasterShaderSet& shaders);
		void linkDescriptorSetLayout(DescriptorSetLayout& layout);

		void enableMultisampling(VkSampleCountFlagBits sampleCount);
		void disableFaceCulling() { m_rasterizer.cullMode = VK_CULL_MODE_NONE; }

	private:
		const Device* m_device;

		// Graphics pipeline state create infos
		VkPipelineVertexInputStateCreateInfo   m_vertexInputInfo{};
		VkPipelineInputAssemblyStateCreateInfo m_inputAssembly{};
		VkPipelineViewportStateCreateInfo      m_viewportState{};
		VkPipelineRasterizationStateCreateInfo m_rasterizer{};
		VkPipelineMultisampleStateCreateInfo   m_multisampling{};
		VkPipelineDepthStencilStateCreateInfo  m_depthStencil{};
		VkPipelineColorBlendAttachmentState    m_colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo    m_colorBlending{};
		VkPipelineDynamicStateCreateInfo       m_dynamicState{};
		VkPushConstantRange                    m_pushConstantRange{};

		std::vector<VkDynamicState> m_dynamicStates{};

		VkPipelineLayoutCreateInfo   m_pipelineLayoutInfo{};
		VkGraphicsPipelineCreateInfo m_pipelineInfo{};
	};

	// Pipeline Class
	VkPipeline       pipeline = VK_NULL_HANDLE;
	VkPipelineLayout layout   = VK_NULL_HANDLE;

	Pipeline(VkPipeline _pipeline, VkPipelineLayout _layout)
		: pipeline(_pipeline), layout(_layout) {}

	void cleanup(const Device& device);
};
