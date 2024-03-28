#include "pch.h"

#include "renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Renderer::beginFrame()
{
	updateUI();

	// Acquire image from swapchain
	m_imageIndex = m_swapchain->acquireImage(m_frameIndex);

	// Compute delta time
	float currentFrameTime = static_cast<float>(glfwGetTime());
	deltaTime       = currentFrameTime - m_lastFrameTime;
	m_lastFrameTime = currentFrameTime;

	m_camera->updatePosition(deltaTime);
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

	// Get window size
	VkExtent2D extent = m_swapchain->getExtent();
	m_windowWidth     = extent.width;
	m_windowHeight    = extent.height;
	aspectRatio       = (float)extent.width / (float)extent.height;

	// Update uniform buffers
	const glm::mat4& view = m_camera->getView();
	const glm::mat4& proj = m_camera->getProjection();
	ubo.viewProjection    = proj * view;
	ubo.viewInverse       = glm::inverse(view);
	ubo.projInverse       = glm::inverse(proj);
	ubo.viewPosition      = m_camera->getPosition();

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
	beginInfo.renderArea.offset = { 0, 0 };
	beginInfo.renderArea.extent = m_swapchain->getExtent();
	beginInfo.renderPass        = m_renderPasses[pass].renderPass;
	beginInfo.clearValueCount   = static_cast<uint32_t>(m_renderPasses[pass].clearValues.size());

	m_renderPasses[pass].clearValues[0].color = { m_ui.backgroundColor[0], m_ui.backgroundColor[1] , m_ui.backgroundColor[2], 1.0f };
	beginInfo.pClearValues      = m_renderPasses[pass].clearValues.data();

	switch (pass)
	{
		case RenderPass::MAIN: beginInfo.framebuffer = m_offScreenFramebuffer->get(); break;
		case RenderPass::POST: beginInfo.framebuffer = m_postFramebuffers[m_imageIndex].get(); break;
	}

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

void Renderer::bindDescriptorSets(Pipeline::PipelineType type)
{
	switch (type)
	{
		case Pipeline::POST:
			vkCmdBindDescriptorSets(
				m_commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				m_pipelines[type].layout,
				0, 1,
				&m_postDescriptorSets[m_frameIndex].getSet(),
				0, nullptr);
			break;

		case Pipeline::LIGHTING:
			vkCmdBindDescriptorSets(
				m_commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				m_pipelines[type].layout,
				0, 1,
				&m_offscreenDescriptorSets[m_frameIndex].getSet(),
				0, nullptr);
			break;

		case Pipeline::FLAT:
			vkCmdBindDescriptorSets(
				m_commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				m_pipelines[type].layout,
				0, 1,
				&m_offscreenDescriptorSets[m_frameIndex].getSet(),
				0, nullptr);
			break;
	}
}

void Renderer::bindPushConstants(Pipeline::PipelineType type)
{
	switch (type)
	{
		case Pipeline::POST:
			vkCmdPushConstants(
				m_commandBuffer,
				m_pipelines[m_pipelineIndex].layout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PostPushConstants),
				&postPushConstants);
			break;

		case Pipeline::LIGHTING:
			vkCmdPushConstants(
				m_commandBuffer,
				m_pipelines[m_pipelineIndex].layout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(MeshPushConstants),
				&pushConstants);
			break;

		case Pipeline::FLAT:
			vkCmdPushConstants(
				m_commandBuffer,
				m_pipelines[m_pipelineIndex].layout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(MeshPushConstants),
				&pushConstants);
			break;
	}
}

void Renderer::bindRtxPipeline()
{
	switch (m_ui.renderMethod)
	{
		case Gui::RenderMethod::RTX_RT:   m_pipelineIndex = Pipeline::RTX_RT;   break;
		case Gui::RenderMethod::RTX_PATH: m_pipelineIndex = Pipeline::RTX_PATH; break;
	}

	vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, m_pipelines[m_pipelineIndex].pipeline);
}

void Renderer::bindRtxDescriptorSets()
{
	std::vector<VkDescriptorSet> rtxSets = { m_rtxDescriptorSets->getSet(), m_offscreenDescriptorSets[m_frameIndex].getSet() };
	vkCmdBindDescriptorSets(
		m_commandBuffer,
		VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
		m_pipelines[m_pipelineIndex].layout,
		0,
		(uint32_t)rtxSets.size(),
		rtxSets.data(),
		0, nullptr);
}

void Renderer::bindRtxPushConstants()
{
	vkCmdPushConstants(
		m_commandBuffer,
		m_pipelines[m_pipelineIndex].layout,
		VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR,
		0,
		sizeof(RtxPushConstants),
		&rtxPushConstants);
}

void Renderer::drawVertex()
{
	if (m_passIndex == RenderPass::POST)
		vkCmdDraw(m_commandBuffer, 3, 1, 0, 0);
	else
		vkCmdDraw(m_commandBuffer, m_vertexBuffer.getCount(), 1, 0, 0);
}

void Renderer::drawIndexed()
{
	vkCmdDrawIndexed(m_commandBuffer, m_indexBuffer.getCount(), 1, 0, 0, 0);
}

void Renderer::drawUI()
{
	if (!m_showUI)
		return;

	m_gui->renderUI(m_commandBuffer);
}

void Renderer::traceRays()
{
	// Update TAA frame
	updateRtxFrame();

	// Stop if max TAA Frame has been reached
	if (m_pipelineIndex == Pipeline::RTX_RT && rtxPushConstants.frame >= m_ui.TAAFrameCount)
		return;

	// Stop if max path tracing frame count has been reached
	if (m_pipelineIndex == Pipeline::RTX_PATH && m_ui.maxPathFrame > 0 && rtxPushConstants.frame >= m_ui.maxPathFrame)
		return;

	// Ray trace
	ShaderBindingTable* sbt = nullptr;
	switch (m_pipelineIndex)
	{
		case Pipeline::RTX_RT:   sbt = m_rtSBT;   break;
		case Pipeline::RTX_PATH: sbt = m_pathSBT; break;
	}
	auto& regions = sbt->getRegions();

	vkCmdTraceRaysKHR(
		m_commandBuffer,
		&regions[ShaderBindingTable::RGEN],
		&regions[ShaderBindingTable::MISS],
		&regions[ShaderBindingTable::HIT],
		&regions[ShaderBindingTable::CALL],
		m_windowWidth, m_windowHeight, 1);
}

void Renderer::setDynamicStates()
{
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_windowWidth;
	viewport.height = (float)m_windowHeight;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = { m_windowWidth, m_windowHeight };
	vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
}

void Renderer::onKeyPress(KeyPressEvent event)
{
	if (event.key == GLFW_KEY_U)
		m_showUI = !m_showUI;
}

void Renderer::updateUI()
{
	if (!m_showUI)
		return;

	// Update UI state
	m_ui = m_gui->getUIState();

	// RTX
	m_useRtx                         = (m_ui.renderMethod != Gui::RenderMethod::RASTER);
	rtxPushConstants.maxDepth        = m_ui.maxDepth;
	rtxPushConstants.sampleCount     = m_ui.sampleCount;
	rtxPushConstants.clearColor      = { m_ui.backgroundColor[0], m_ui.backgroundColor[1], m_ui.backgroundColor[2], 1.0f };
	rtxPushConstants.russianRoulette = m_ui.russianRoulette;
	if (m_ui.changed)
		resetRtxFrame();

	// Light
	ubo.lightColor     = { m_ui.lightColor[0], m_ui.lightColor[1], m_ui.lightColor[2] };
	ubo.lightPosition  = { m_ui.lightPosition[0], m_ui.lightPosition[1], m_ui.lightPosition[2] };
	ubo.lightIntensity = m_ui.lightIntensity;

	// Scene
	postPushConstants.exposure = m_ui.exposure;

	// Camera
	m_camera->updateSensitivity(m_ui.sensitivity);
	m_camera->updateSpeed(m_ui.speed);
	m_camera->updateMode(m_ui.mode);
	m_camera->saveCamera(m_ui.cameraSaves);
	m_camera->switchCameras(m_ui.currentCamera);

	// Debug
	ubo.debugMode = (int)m_ui.debugMode;

	// Start UI
	m_gui->beginUI();
}

void Renderer::updateRtxFrame()
{
	const glm::mat4& newView = m_camera->getView();

	if (m_currentCameraView != newView)
	{
		resetRtxFrame();
		m_currentCameraView = newView;
	}

	rtxPushConstants.frame++;
}

void Renderer::resetRtxFrame()
{
	rtxPushConstants.frame = -1;
}
