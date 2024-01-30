#include "renderer.h"

void Renderer::beginFrame(Swapchain& swapchain, CommandManager& commandManager, uint32_t frame, uint32_t* index)
{
	// Acquire image from swapchain
	int imageIndex = -1;
	while (imageIndex < 0)
		imageIndex = swapchain.acquireImage(frame);

	*index = imageIndex;

	// Reset and begin command buffer
	VkCommandBuffer commandBuffer = commandManager.getCommandBuffer(frame);
	vkResetCommandBuffer(commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("Failed to begin recording command buffer");

	// Update dynamic states
	VkExtent2D extent = swapchain.getExtent();

	VkViewport viewport{};
	viewport.x        = 0.0f;
	viewport.y        = 0.0f;
	viewport.width    = (float)extent.width;
	viewport.height   = (float)extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::beginRenderPass(RenderPass::Manager& renderPassManager, CommandManager& commandManager, Swapchain& swapchain, uint32_t frameIndex, uint32_t passIndex, uint32_t& imageIndex)
{
	renderPassManager.beginPass(
		passIndex,
		swapchain.getFramebuffer(imageIndex),
		swapchain.getExtent(),
		commandManager.getCommandBuffer(frameIndex));
}

void Renderer::endRenderPass(CommandManager& commandManager, uint32_t frameIndex)
{
	vkCmdEndRenderPass(commandManager.getCommandBuffer(frameIndex));
}

void Renderer::draw(CommandManager& commandManager, Buffer& vertexBuffer, uint32_t frameIndex)
{
	VkCommandBuffer commandBuffer = commandManager.getCommandBuffer(frameIndex);

	VkBuffer vertexBuffers[] = { vertexBuffer.getBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

void Renderer::submit(Swapchain& swapchain, CommandManager& commandManager, uint32_t frame)
{
	swapchain.submitGraphics(commandManager.getCommandBuffer(frame), frame);
}

void Renderer::endFrame(Swapchain& swapchain, uint32_t frame, uint32_t& index)
{
	swapchain.present(frame, index);
}
