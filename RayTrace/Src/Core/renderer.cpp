#include "pch.h"

#include "renderer.h"

void Renderer::BeginFrame(RenderingContext& ctx)
{
	// Acquire image from swapchain
	ctx.imageIndex = ctx.swapchain.acquireImage(ctx.frameIndex);

	// Reset and begin command buffer
	ctx.commandBuffer = ctx.commandSystem.getCommandBuffer(ctx.frameIndex);
	vkResetCommandBuffer(ctx.commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(ctx.commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to begin recording command buffer");
		throw;
	}

	// Update dynamic states
	VkExtent2D extent = ctx.swapchain.getExtent();

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(ctx.commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;
	vkCmdSetScissor(ctx.commandBuffer, 0, 1, &scissor);
}

void Renderer::Submit(RenderingContext& ctx)
{
	ctx.swapchain.submitGraphics(ctx.commandBuffer, ctx.frameIndex);
}

void Renderer::EndFrame(RenderingContext& ctx)
{
	ctx.swapchain.present(ctx.frameIndex, ctx.imageIndex);

	// Update frame
	ctx.frameIndex = (ctx.frameIndex + 1) % ctx.framesInFlight;
}

void Renderer::BeginRenderPass(RenderingContext& ctx, RenderPass::PassType pass)
{
	VkRenderPassBeginInfo beginInfo{};
	beginInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginInfo.renderPass        = ctx.renderPasses[pass].renderPass;
	beginInfo.framebuffer       = ctx.swapchain.getFramebuffer(ctx.imageIndex);
	beginInfo.renderArea.offset = { 0, 0 };
	beginInfo.renderArea.extent = ctx.swapchain.getExtent();
	beginInfo.clearValueCount   = static_cast<uint32_t>(ctx.renderPasses[pass].clearValues.size());
	beginInfo.pClearValues      = ctx.renderPasses[pass].clearValues.data();

	vkCmdBeginRenderPass(ctx.commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::EndRenderPass(RenderingContext& ctx)
{
	vkCmdEndRenderPass(ctx.commandBuffer);
}

void Renderer::BindPipeline(RenderingContext& ctx, Pipeline::PipelineType pipeline)
{
	vkCmdBindPipeline(ctx.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ctx.pipelines[pipeline].pipeline);
}

void Renderer::BindVertexBuffer(RenderingContext& ctx, Buffer& vertexBuffer)
{
	VkBuffer vertexBuffers[] = { vertexBuffer.getBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(ctx.commandBuffer, 0, 1, vertexBuffers, offsets);
}

void Renderer::BindIndexBuffer(RenderingContext& ctx, Buffer& indexBuffer)
{
	vkCmdBindIndexBuffer(ctx.commandBuffer, indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void Renderer::DrawVertex(RenderingContext& ctx, Buffer& vertexBuffer)
{
	vkCmdDraw(ctx.commandBuffer, vertexBuffer.getCount(), 1, 0, 0);
}

void Renderer::DrawIndexed(RenderingContext& ctx, Buffer& indexBuffer)
{
	vkCmdDrawIndexed(ctx.commandBuffer, indexBuffer.getCount(), 1, 0, 0, 0);
}