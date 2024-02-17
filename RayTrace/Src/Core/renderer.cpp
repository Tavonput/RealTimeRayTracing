#include "pch.h"

#include "renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Renderer::beginFrame()
{
	// Acquire image from swapchain
	m_imageIndex = m_swapchain->acquireImage(m_frameIndex);

	// Compute delta time
	float currentFrameTime = static_cast<float>(glfwGetTime());
	deltaTime = currentFrameTime - m_lastFrameTime;
	m_lastFrameTime = currentFrameTime;

	// Reset and begin command buffer
	m_commandBuffer = m_commandSystem->getCommandBuffer(m_frameIndex);
	vkResetCommandBuffer(m_commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(m_commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to begin recording command buffer");
		throw;
	}

	// Update dynamic states
	VkExtent2D extent = m_swapchain->getExtent();

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;
	vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

	aspectRatio = extent.width / (float)extent.height;

	// Update uniform buffers
	ubo.viewProjection = m_camera->getViewProjection(aspectRatio);
	ubo.viewPosition   = m_camera->getPosition();

	Buffer::Update(BufferType::UNIFORM, m_uniformBuffers[m_frameIndex], &ubo);
}

void Renderer::submit()
{
	m_swapchain->submitGraphics(m_commandBuffer, m_frameIndex);
}

void Renderer::endFrame()
{
	m_swapchain->present(m_frameIndex, m_imageIndex);

	// Update frame
	m_frameIndex = (m_frameIndex + 1) % m_framesInFlight;
}

void Renderer::beginRenderPass(RenderPass::PassType pass)
{
	VkRenderPassBeginInfo beginInfo{};
	beginInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginInfo.renderPass        = m_renderPasses[pass].renderPass;
	beginInfo.framebuffer       = m_swapchain->getFramebuffer(m_imageIndex);
	beginInfo.renderArea.offset = { 0, 0 };
	beginInfo.renderArea.extent = m_swapchain->getExtent();
	beginInfo.clearValueCount   = static_cast<uint32_t>(m_renderPasses[pass].clearValues.size());
	beginInfo.pClearValues      = m_renderPasses[pass].clearValues.data();

	vkCmdBeginRenderPass(m_commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

	m_passIndex = pass;
}

void Renderer::endRenderPass()
{
	vkCmdEndRenderPass(m_commandBuffer);
}

void Renderer::bindPipeline(Pipeline::PipelineType pipeline)
{
	vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines[pipeline].pipeline);

	m_pipelineIndex = pipeline;
}

void Renderer::bindVertexBuffer(Buffer& vertexBuffer)
{
	VkBuffer vertexBuffers[] = { vertexBuffer.getBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, vertexBuffers, offsets);

	m_vertexBuffer = vertexBuffer;
}

void Renderer::bindIndexBuffer(Buffer& indexBuffer)
{
	vkCmdBindIndexBuffer(m_commandBuffer, indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);

	m_indexBuffer = indexBuffer;
}

void Renderer::bindDescriptorSets()
{
	vkCmdBindDescriptorSets(
		m_commandBuffer, 
		VK_PIPELINE_BIND_POINT_GRAPHICS, 
		m_pipelines[m_pipelineIndex].layout, 
		0, 1, 
		&m_descriptorSets[m_frameIndex].getSet(), 
		0, nullptr);
}

void Renderer::bindPushConstants()
{
	vkCmdPushConstants(
		m_commandBuffer, 
		m_pipelines[m_pipelineIndex].layout, 
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
		0, 
		sizeof(MeshPushConstants), 
		&pushConstants);
}

void Renderer::drawVertex()
{
	vkCmdDraw(m_commandBuffer, m_vertexBuffer.getCount(), 1, 0, 0);
}

void Renderer::drawIndexed()
{
	vkCmdDrawIndexed(m_commandBuffer, m_indexBuffer.getCount(), 1, 0, 0, 0);

	/*ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDebugLogWindow();

	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_commandBuffer, m_pipelines[m_pipelineIndex].pipeline);*/

}