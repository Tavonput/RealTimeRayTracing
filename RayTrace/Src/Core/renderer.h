#pragma once

#include <vulkan/vulkan.h>

#include "device.h"
#include "swapchain.h"
#include "command_manager.h"
#include "render_pass.h"
#include "buffer.h"
#include "pipeline.h"

struct RenderingContext
{
	// Provided by the application
	Swapchain&           swapchain;
	CommandManager&      commandManager;
	RenderPass::Manager& renderPassManager;
	Pipeline&            pipeline;

	uint32_t framesInFlight = 1;

	// Handled by the renderer
	uint32_t frameIndex = 0;
	uint32_t imageIndex = 0;

	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	// Constructor
	RenderingContext(
		Swapchain& _swapchain, 
		CommandManager& _commandManager, 
		RenderPass::Manager& _renderPassManager, 
		Pipeline& _pipeline, 
		uint32_t _framesInFlight
	)
		: swapchain(_swapchain), 
		  commandManager(_commandManager), 
		  renderPassManager(_renderPassManager), 
		  pipeline(_pipeline), 
		  framesInFlight(_framesInFlight)
	{}
};

class Renderer
{
public:
	static void BeginFrame(RenderingContext& ctx);
	static void Submit(RenderingContext& ctx);
	static void EndFrame(RenderingContext& ctx);

	static void BeginRenderPass(RenderingContext& ctx, uint32_t passIndex);
	static void EndRenderPass(RenderingContext& ctx);

	static void BindPipeline(RenderingContext& ctx);

	static void Draw(RenderingContext& ctx, Buffer& vertexBuffer);
};