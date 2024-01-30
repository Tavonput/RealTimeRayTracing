#pragma once

#include <vulkan/vulkan.h>

#include "device.h"
#include "swapchain.h"
#include "command_manager.h"
#include "render_pass.h"
#include "buffer.h"

class Renderer
{
public:
	static void beginFrame(Swapchain& swapchain, CommandManager& commandManager, uint32_t frame, uint32_t* index);
	static void submit(Swapchain& swapchain, CommandManager& commandManager, uint32_t frame);
	static void endFrame(Swapchain& swapchain, uint32_t frame, uint32_t& index);

	static void beginRenderPass(RenderPass::Manager& renderPassManager, CommandManager& commandManager, Swapchain& swapchain, uint32_t frameIndex, uint32_t passIndex, uint32_t& imageIndex);
	static void endRenderPass(CommandManager& commandManager, uint32_t frameIndex);
	static void draw(CommandManager& commandManager, Buffer& vertexBuffer, uint32_t frameIndex);
};