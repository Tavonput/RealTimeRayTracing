#pragma once

#include "Application/logging.h"
#include "Application/camera.h"

#include "device.h"
#include "swapchain.h"
#include "command.h"
#include "render_pass.h"
#include "buffer.h"
#include "pipeline.h"
#include "rendering_structures.h"
#include "descriptor.h"

//#include "ImGui/imconfig.h"
//#include "ImGui/imgui_tables.cpp"
//#include "ImGui/imgui_internal.h"
//#include "ImGui/imgui.cpp"
//#include "ImGui/imgui_draw.cpp"
//#include "ImGui/imgui_widgets.cpp"
//#include "ImGui/imgui_demo.cpp"
//#include "ImGui/imgui.h"
//#include "ImGui/imgui_impl_glfw.h"
//#include "ImGui/imgui_impl_vulkan.h"

class Renderer
{
public:
	// Create info
	struct CreateInfo
	{
		Swapchain*     pSwapchain;
		CommandSystem* pCommandSystem;
		RenderPass*    pRenderPasses;
		Pipeline*      pPipelines;
		Buffer*        pUniformBuffers;
		DescriptorSet* pDescriptorSets;
		Camera*        pCamera;

		uint32_t framesInFlight = 2;
	};

	GlobalUniform     ubo;
	MeshPushConstants pushConstants;

	float deltaTime   = 0.0f;
	float aspectRatio = 0.0f;

	Renderer(Renderer::CreateInfo info)
		: m_swapchain     (info.pSwapchain),
		  m_commandSystem (info.pCommandSystem),
		  m_renderPasses  (info.pRenderPasses),
		  m_pipelines     (info.pPipelines),
		  m_uniformBuffers(info.pUniformBuffers),
		  m_descriptorSets(info.pDescriptorSets),
		  m_camera        (info.pCamera),
		  m_framesInFlight(info.framesInFlight)
	{}

	void beginFrame();
	void submit();
	void endFrame();

	void beginRenderPass(RenderPass::PassType pass);
	void endRenderPass();

	void bindPipeline(Pipeline::PipelineType pipeline);
	void bindVertexBuffer(Buffer& vertexBuffer);
	void bindIndexBuffer(Buffer& indexBuffer);
	void bindDescriptorSets();
	void bindPushConstants();

	void drawVertex();
	void drawIndexed();

private:
	Swapchain*     m_swapchain      = nullptr;
	CommandSystem* m_commandSystem  = nullptr;
	RenderPass*    m_renderPasses   = nullptr;
	Pipeline*      m_pipelines      = nullptr;
	Buffer*        m_uniformBuffers = nullptr;
	DescriptorSet* m_descriptorSets = nullptr;
	Camera*        m_camera         = nullptr;

	RenderPass::PassType   m_passIndex     = RenderPass::MAIN;
	Pipeline::PipelineType m_pipelineIndex = Pipeline::LIGHTING;

	uint32_t m_frameIndex     = 0;
	uint32_t m_imageIndex     = 0;
	uint32_t m_framesInFlight = 2;
	float    m_lastFrameTime  = 0.0f;

	VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;

	Buffer m_vertexBuffer;
	Buffer m_indexBuffer;
};
