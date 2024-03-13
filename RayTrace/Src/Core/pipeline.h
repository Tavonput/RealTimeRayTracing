#pragma once

#include "Application/logging.h"
#include "device.h"
#include "shader.h"
#include "rendering_structures.h"
#include "render_pass.h"
#include "descriptor.h"
#include "buffer.h"

/*****************************************************************************************************************
 * 
 * @class Pipeline
 * 
 *
 */
class Pipeline 
{
public:

	enum PipelineType
	{
		LIGHTING = 0,
		FLAT,
		POST,
		RTX
	};

	// Builder Class
	class Builder
	{
	public:
		Builder(const Device& device);

		Pipeline buildGraphicsPipeline(Pipeline::PipelineType type, const std::string name);
		Pipeline buildRtxPipeline(const std::string name);
		void reset();

		void addGraphicsBase();
		void addRtxBase();

		void linkRenderPass(RenderPass& pass);
		void linkShaders(ShaderSet& shaders);
		void linkDescriptorSetLayouts(VkDescriptorSetLayout* layouts, uint32_t count);
		void linkPushConstants(uint32_t size);

		void linkRtxPushConstants(uint32_t size);
		void linkRtxShaders(ShaderSet& shaders);

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

		VkRayTracingPipelineCreateInfoKHR m_rtxPipelineInfo{};
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

/*****************************************************************************************************************
 *
 * @class Shader Binding Table
 *
 * Maybe this should go in the shader file and be built off a ShaderSet.
 * 
 */
class ShaderBindingTable
{
public:
	enum SbtRegion
	{
		RGEN = 0,
		MISS,
		HIT,
		CALL
	};

	void build(const Device& device, VkPipeline& rtxPipeline);

	void cleanup();

	std::array<VkStridedDeviceAddressRegionKHR, 4>& getRegions() { return m_regions; }

private:
	const Device* m_device = nullptr;

	Buffer m_sbtBuffer;

	std::array<VkStridedDeviceAddressRegionKHR, 4> m_regions;

	uint32_t alignUp(uint32_t x, uint32_t y);
};
