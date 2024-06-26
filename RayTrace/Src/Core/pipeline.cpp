#include "pch.h"

#include "pipeline.h"

/*****************************************************************************************************************
 *
 * Pipeline Builder
 *
 */
Pipeline::Builder::Builder(const Device& device)
{
	m_device = &device;
}

Pipeline Pipeline::Builder::buildGraphicsPipeline(Pipeline::PipelineType type, const std::string name)
{
	APP_LOG_INFO("Building pipeline ({})", name);

	// Vertex buffer
	auto bindingDescription    = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	m_vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	m_vertexInputInfo.vertexBindingDescriptionCount   = 1;
	m_vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
	m_vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	m_vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

	// Go buffer-less if post
	if (type == Pipeline::POST)
	{
		m_vertexInputInfo.vertexBindingDescriptionCount   = 0;
		m_vertexInputInfo.pVertexBindingDescriptions      = nullptr;
		m_vertexInputInfo.vertexAttributeDescriptionCount = 0;
		m_vertexInputInfo.pVertexAttributeDescriptions    = nullptr;
	}

	// Build layout
	VkPipelineLayout layout;
	if (vkCreatePipelineLayout(m_device->getLogical(), &m_pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create pipeline layout ({})", name);
		throw;
	}
	m_pipelineInfo.layout = layout;

	// Build pipeline
	VkPipeline pipeline;
	if (vkCreateGraphicsPipelines(m_device->getLogical(), VK_NULL_HANDLE, 1, &m_pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create pipeline ({})", name);
		throw;
	}

	return Pipeline(pipeline, layout, name);
}

Pipeline Pipeline::Builder::buildRtxPipeline(const std::string name)
{
	APP_LOG_INFO("Building pipeline ({})", name);

	// Build layout
	VkPipelineLayout layout;
	if (vkCreatePipelineLayout(m_device->getLogical(), &m_pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create pipeline layout ({})", name);
		throw;
	}
	m_rtxPipelineInfo.layout = layout;

	// Build pipeline
	VkPipeline pipeline;
	if (vkCreateRayTracingPipelinesKHR(m_device->getLogical(), {}, {}, 1, &m_rtxPipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create pipeline ({})", name);
		throw;
	}

	return Pipeline(pipeline, layout, name);
}

void Pipeline::Builder::reset()
{
	// Reset all of the structures to empty
	m_vertexInputInfo      = VkPipelineVertexInputStateCreateInfo{};
	m_inputAssembly        = VkPipelineInputAssemblyStateCreateInfo{};
	m_viewportState        = VkPipelineViewportStateCreateInfo{};
	m_rasterizer           = VkPipelineRasterizationStateCreateInfo{};
	m_multisampling        = VkPipelineMultisampleStateCreateInfo{};
	m_depthStencil         = VkPipelineDepthStencilStateCreateInfo{};
	m_colorBlendAttachment = VkPipelineColorBlendAttachmentState{};
	m_colorBlending        = VkPipelineColorBlendStateCreateInfo{};
	m_dynamicState         = VkPipelineDynamicStateCreateInfo{};
	m_pushConstantRange    = VkPushConstantRange{};
	m_pipelineLayoutInfo   = VkPipelineLayoutCreateInfo{};
	m_pipelineInfo         = VkGraphicsPipelineCreateInfo{};
}

void Pipeline::Builder::addGraphicsBase()
{
	// Assembly
	m_inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	m_inputAssembly.primitiveRestartEnable = VK_FALSE;

	// Viewport
	m_viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	m_viewportState.viewportCount = 1;
	m_viewportState.scissorCount  = 1;

	// Rasterization
	m_rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_rasterizer.depthClampEnable        = VK_FALSE;
	m_rasterizer.rasterizerDiscardEnable = VK_FALSE;
	m_rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
	m_rasterizer.lineWidth               = 1.0f;
	m_rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
	m_rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	m_rasterizer.depthBiasEnable         = VK_FALSE;

	// Multisampling
	m_multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_multisampling.sampleShadingEnable  = VK_FALSE;
	m_multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Depth stencil
	m_depthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_depthStencil.depthTestEnable       = VK_TRUE;
	m_depthStencil.depthWriteEnable      = VK_TRUE;
	m_depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS;
	m_depthStencil.depthBoundsTestEnable = VK_FALSE;
	m_depthStencil.minDepthBounds        = 0.0f;
	m_depthStencil.maxDepthBounds        = 1.0f;
	m_depthStencil.stencilTestEnable     = VK_FALSE;
	m_depthStencil.front                 = {};
	m_depthStencil.back                  = {};

	// Blend attachment
	m_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	m_colorBlendAttachment.blendEnable    = VK_FALSE;

	// Blending
	m_colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_colorBlending.logicOpEnable     = VK_FALSE;
	m_colorBlending.logicOp           = VK_LOGIC_OP_COPY;
	m_colorBlending.attachmentCount   = 1;
	m_colorBlending.pAttachments      = &m_colorBlendAttachment;
	m_colorBlending.blendConstants[0] = 0.0f;
	m_colorBlending.blendConstants[1] = 0.0f;
	m_colorBlending.blendConstants[2] = 0.0f;
	m_colorBlending.blendConstants[3] = 0.0f;

	// Dynamic states
	m_dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	m_dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	m_dynamicState.dynamicStateCount = static_cast<uint32_t>(m_dynamicStates.size());
	m_dynamicState.pDynamicStates    = m_dynamicStates.data();

	// Pipeline layout
	m_pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	m_pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	m_pipelineInfo.pInputAssemblyState = &m_inputAssembly;
	m_pipelineInfo.pVertexInputState   = &m_vertexInputInfo;
	m_pipelineInfo.pViewportState      = &m_viewportState;
	m_pipelineInfo.pRasterizationState = &m_rasterizer;
	m_pipelineInfo.pMultisampleState   = &m_multisampling;
	m_pipelineInfo.pDepthStencilState  = &m_depthStencil;
	m_pipelineInfo.pColorBlendState    = &m_colorBlending;
	m_pipelineInfo.pDynamicState       = &m_dynamicState;
	m_pipelineInfo.subpass             = 0;
	m_pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;
}

void Pipeline::Builder::addRtxBase()
{
	m_pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	m_rtxPipelineInfo.sType    = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;

	VkPhysicalDeviceRayTracingPipelinePropertiesKHR props = m_device->getRtxProperties();
	m_rtxPipelineInfo.maxPipelineRayRecursionDepth = 2;
	
	if (props.maxRayRecursionDepth <= 1)
	{
		APP_LOG_CRITICAL("GPU does not support ray recursion");
		throw std::exception();
	}
}

void Pipeline::Builder::linkRenderPass(RenderPass& pass)
{
	m_pipelineInfo.renderPass = pass.renderPass;
}

void Pipeline::Builder::linkShaders(ShaderSet& shaders)
{
	m_pipelineInfo.stageCount = 2;
	m_pipelineInfo.pStages    = shaders.getStages();
}

void Pipeline::Builder::linkDescriptorSetLayouts(VkDescriptorSetLayout* layouts, uint32_t count)
{
	m_pipelineLayoutInfo.setLayoutCount = count;
	m_pipelineLayoutInfo.pSetLayouts    = layouts;
}

void Pipeline::Builder::linkPushConstants(uint32_t size)
{
	m_pushConstantRange.offset     = 0;
	m_pushConstantRange.size       = size;
	m_pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	m_pipelineLayoutInfo.pPushConstantRanges    = &m_pushConstantRange;
	m_pipelineLayoutInfo.pushConstantRangeCount = 1;
}

void Pipeline::Builder::enableMultisampling(VkSampleCountFlagBits sampleCount)
{
	m_multisampling.sampleShadingEnable  = VK_TRUE;
	m_multisampling.minSampleShading     = 0.2f;
	m_multisampling.rasterizationSamples = sampleCount;
}

void Pipeline::Builder::linkRtxPushConstants(uint32_t size)
{
	m_pushConstantRange.offset     = 0;
	m_pushConstantRange.size       = size;
	m_pushConstantRange.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR;

	m_pipelineLayoutInfo.pPushConstantRanges    = &m_pushConstantRange;
	m_pipelineLayoutInfo.pushConstantRangeCount = 1;
}

void Pipeline::Builder::linkRtxShaders(ShaderSet& shaders)
{
	m_rtxPipelineInfo.stageCount = shaders.getStageCount();
	m_rtxPipelineInfo.pStages    = shaders.getStages();

	m_rtxPipelineInfo.groupCount = shaders.getGroupCount();
	m_rtxPipelineInfo.pGroups    = shaders.getShaderGroup();
}

/*****************************************************************************************************************
 *
 * Pipeline
 *
 */
void Pipeline::cleanup(const Device& device)
{
	APP_LOG_INFO("Destroying pipeline ({})", m_name);

	vkDestroyPipeline(device.getLogical(), pipeline, nullptr);
	vkDestroyPipelineLayout(device.getLogical(), layout, nullptr);
}
