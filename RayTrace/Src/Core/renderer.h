#pragma once

#include "device.h"
#include "swapchain.h"
#include "command.h"
#include "render_pass.h"
#include "buffer.h"
#include "pipeline.h"

struct RenderingContext
{
	// Provided by the application
	Swapchain&           swapchain;
	CommandSystem&      commandSystem;
	std::vector<RenderPass>& renderPasses;
	std::vector<Pipeline>&   pipelines;

	uint32_t framesInFlight = 1;

	// Handled by the renderer
	uint32_t frameIndex = 0;
	uint32_t imageIndex = 0;

	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	// Constructor
	RenderingContext(
		Swapchain&           _swapchain, 
		CommandSystem&      _commandSystem, 
		std::vector<RenderPass>& _renderPasses,
		std::vector<Pipeline>&   _pipelines,
		uint32_t             _framesInFlight
	)
		: swapchain        (_swapchain), 
		  commandSystem   (_commandSystem), 
		  renderPasses     (_renderPasses),
		  pipelines        (_pipelines), 
		  framesInFlight   (_framesInFlight)
	{}
};

class Renderer
{
public:
	static void BeginFrame(RenderingContext& ctx);
	static void Submit(RenderingContext& ctx);
	static void EndFrame(RenderingContext& ctx);

	static void BeginRenderPass(RenderingContext& ctx, RenderPass::PassType pass);
	static void EndRenderPass(RenderingContext& ctx);

	static void BindPipeline(RenderingContext& ctx, Pipeline::PipelineType pipeline);
	static void BindVertexBuffer(RenderingContext& ctx, Buffer& vertexBuffer);
	static void BindIndexBuffer(RenderingContext& ctx, Buffer& indexBuffer);

	static void DrawVertex(RenderingContext& ctx, Buffer& vertexBuffer);
	static void DrawIndexed(RenderingContext& ctx, Buffer& indexBuffer);
};