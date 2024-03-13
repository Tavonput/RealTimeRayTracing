#pragma once

#include "Application/logging.h"
#include "Application/camera.h"
#include "Application/Gui.h"

#include "device.h"
#include "swapchain.h"
#include "command.h"
#include "render_pass.h"
#include "buffer.h"
#include "pipeline.h"
#include "rendering_structures.h"
#include "descriptor.h"
#include "framebuffer.h"


class Renderer
{
public:
	// Create info
	struct CreateInfo
	{
		Swapchain*     pSwapchain               = nullptr;
		CommandSystem* pCommandSystem           = nullptr;
		RenderPass*    pRenderPasses            = nullptr;
		Pipeline*      pPipelines               = nullptr;
		Buffer*        pUniformBuffers          = nullptr;
		DescriptorSet* pOffscreenDescriptorSets = nullptr;
		DescriptorSet* pPostDescriptorSets      = nullptr;
		DescriptorSet* pRtxDescriptorSets       = nullptr;
		Camera*        pCamera                  = nullptr;
		Gui*           pGui                     = nullptr;

		ShaderBindingTable* pSBT = nullptr;

		Framebuffer* pPostFramebuffers     = nullptr;
		Framebuffer* pOffscreenFramebuffer = nullptr;

		uint32_t framesInFlight = 2;

		bool enableRtx = false;
	};

	GlobalUniform     ubo;
	MeshPushConstants pushConstants;
	RtxPushConstants  rtxPushConstants;

	float deltaTime   = 0.0f;
	float aspectRatio = 0.0f;

	Renderer(Renderer::CreateInfo info)
		: m_swapchain              (info.pSwapchain),
		  m_commandSystem          (info.pCommandSystem),
		  m_renderPasses           (info.pRenderPasses),
		  m_pipelines              (info.pPipelines),
		  m_uniformBuffers         (info.pUniformBuffers),
		  m_offscreenDescriptorSets(info.pOffscreenDescriptorSets),
		  m_postDescriptorSets     (info.pPostDescriptorSets),
		  m_rtxDescriptorSets      (info.pRtxDescriptorSets),
		  m_camera                 (info.pCamera),
		  m_framesInFlight         (info.framesInFlight),
          m_gui                    (info.pGui),
		  m_postFramebuffers       (info.pPostFramebuffers),
		  m_offScreenFramebuffer   (info.pOffscreenFramebuffer),
		  m_SBT                    (info.pSBT),
		  m_useRtx                 (info.enableRtx)
	{}

	void beginFrame();
	void submit();
	void endFrame();

	void beginRenderPass(RenderPass::PassType pass);
	void endRenderPass();

	void bindPipeline(Pipeline::PipelineType pipeline);
	void bindVertexBuffer(Buffer& vertexBuffer);
	void bindIndexBuffer(Buffer& indexBuffer);
	void bindDescriptorSets(Pipeline::PipelineType type);
	void bindPushConstants();
	void bindRtxPushConstants();

	void drawVertex();
	void drawIndexed();
	void drawUI();

	void traceRays();

	void setDynamicStates();

	bool isRtxEnabled() const { return m_useRtx; }

private:
	Swapchain*     m_swapchain               = nullptr;
	CommandSystem* m_commandSystem           = nullptr;
	RenderPass*    m_renderPasses            = nullptr;
	Pipeline*      m_pipelines               = nullptr;
	Buffer*        m_uniformBuffers          = nullptr;
	DescriptorSet* m_offscreenDescriptorSets = nullptr;
	DescriptorSet* m_postDescriptorSets      = nullptr;
	DescriptorSet* m_rtxDescriptorSets       = nullptr;
	Camera*        m_camera                  = nullptr;
	Gui*           m_gui                     = nullptr;

	RenderPass::PassType   m_passIndex     = RenderPass::MAIN;
	Pipeline::PipelineType m_pipelineIndex = Pipeline::LIGHTING;

	Framebuffer* m_postFramebuffers     = nullptr;
	Framebuffer* m_offScreenFramebuffer = nullptr;

	uint32_t m_frameIndex     = 0;
	uint32_t m_imageIndex     = 0;
	uint32_t m_framesInFlight = 2;
	float    m_lastFrameTime  = 0.0f;

	uint32_t m_windowWidth  = 0;
	uint32_t m_windowHeight = 0;

	VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;

	Buffer m_vertexBuffer;
	Buffer m_indexBuffer;

	ShaderBindingTable* m_SBT = nullptr;

	bool m_useRtx = false;
};
