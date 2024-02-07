#include "pch.h"

#include "renderer.h"

void Renderer::BeginFrame(RenderingContext& ctx)
{
	// Acquire image from swapchain
	ctx.imageIndex = ctx.swapchain.acquireImage(ctx.frameIndex);

	// Reset and begin command buffer
	ctx.commandBuffer = ctx.commandManager.getCommandBuffer(ctx.frameIndex);
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

void Renderer::BeginRenderPass(RenderingContext& ctx, uint32_t passIndex)
{
	ctx.renderPassManager.beginPass(
		passIndex,
		ctx.swapchain.getFramebuffer(ctx.imageIndex),
		ctx.swapchain.getExtent(),
		ctx.commandBuffer);
}

void Renderer::EndRenderPass(RenderingContext& ctx)
{
	vkCmdEndRenderPass(ctx.commandBuffer);
}

void Renderer::BindPipeline(RenderingContext& ctx, uint32_t pipelineIndex)
{
	ctx.pipelineManager.bindPipeline(pipelineIndex, ctx.commandBuffer);
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