#include "pipeline.h"

// -----------------------------------------------------
// -----------------------------------------------------
// Builder
// -----------------------------------------------------
// -----------------------------------------------------

Pipeline::Builder::Builder(const Device& device, Logger logger)
{
	m_device = &device;
	m_logger = logger;
}

Pipeline Pipeline::Builder::buildPipeline(const char* vertexShaderPath, const char* fragmentShaderPath, VkRenderPass& renderPass)
{
	LOG_INFO("Building pipeline");

	// Shaders
	RasterShaderSet shaders(vertexShaderPath, fragmentShaderPath, *m_device, m_logger);

	// Vertex buffer
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount   = 1;
	vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

	// Assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	// Viewport
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount  = 1;

	// Rasterization
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable        = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth               = 1.0f;
	rasterizer.cullMode                = VK_CULL_MODE_FRONT_BIT;
	rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable         = VK_FALSE;

	// Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable  = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Depth stencil
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable       = VK_FALSE;
	depthStencil.depthWriteEnable      = VK_TRUE;
	depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds        = 0.0f;
	depthStencil.maxDepthBounds        = 1.0f;
	depthStencil.stencilTestEnable     = VK_FALSE;
	depthStencil.front                 = {};
	depthStencil.back                  = {};

	// Blend attachment
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable    = VK_FALSE;

	// Blending
	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable     = VK_FALSE;
	colorBlending.logicOp           = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount   = 1;
	colorBlending.pAttachments      = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	// Dynamic states
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates    = dynamicStates.data();

	// Pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount         = 0;
	pipelineLayoutInfo.pSetLayouts            = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	VkPipelineLayout layout;
	if (vkCreatePipelineLayout(m_device->getLogical(), &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS)
	{
		LOG_CRITICAL("Failed to create pipeline layout");
		throw;
	}

	// Pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount          = 2;
	pipelineInfo.pStages             = shaders.getStages();
	pipelineInfo.pVertexInputState   = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState      = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState   = &multisampling;
	pipelineInfo.pDepthStencilState  = &depthStencil;
	pipelineInfo.pColorBlendState    = &colorBlending;
	pipelineInfo.pDynamicState       = &dynamicState;
	pipelineInfo.layout              = layout;
	pipelineInfo.renderPass          = renderPass;
	pipelineInfo.subpass             = 0;
	pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

	VkPipeline pipeline;
	if (vkCreateGraphicsPipelines(m_device->getLogical(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
	{
		LOG_CRITICAL("Failed to create pipeline");
		throw;
	}

	shaders.cleanup();

	LOG_INFO("Pipeling build successful");

	return Pipeline(pipeline, layout);
}

// -----------------------------------------------------
// -----------------------------------------------------
// Manager
// -----------------------------------------------------
// -----------------------------------------------------

void Pipeline::Manager::init(const Device& device, Logger logger)
{
	m_device = &device;
	m_logger = logger;
}

void Pipeline::Manager::addPipeline(Pipeline pipeline)
{
	m_pipelines.push_back(pipeline);
}

void Pipeline::Manager::bindPipeline(uint32_t index, VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines[index].pipeline);
}

void Pipeline::Manager::cleanup()
{
	LOG_INFO("Destroying pipelines");

	for (auto& pipeline : m_pipelines)
	{
		vkDestroyPipeline(m_device->getLogical(), pipeline.pipeline, nullptr);
		vkDestroyPipelineLayout(m_device->getLogical(), pipeline.layout, nullptr);
	}
}
