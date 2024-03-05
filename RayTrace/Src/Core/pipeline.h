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
		LIGHTING = 0,
		FLAT = 1,
		POST = 2
	};

	// Builder Class
	class Builder
	{
	public:
		Builder(const Device& device);

		Pipeline buildPipeline(Pipeline::PipelineType type, const std::string name);
		void reset();

		void addGraphicsBase();

		void linkRenderPass(RenderPass& pass);
		void linkShaders(RasterShaderSet& shaders);
		void linkDescriptorSetLayout(DescriptorSetLayout& layout);
		void linkPushConstants(uint32_t size);

		void enableMultisampling(VkSampleCountFlagBits sampleCount);
		void disableFaceCulling() { m_rasterizer.cullMode = VK_CULL_MODE_NONE; }
		void disableDepthTesting() { m_depthStencil.depthTestEnable = VK_FALSE; }

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

	Pipeline(VkPipeline _pipeline, VkPipelineLayout _layout, const std::string name)
		: pipeline(_pipeline), layout(_layout), m_name(name) {}

	void cleanup(const Device& device);

private:
	std::string m_name = "";
};
