#include "pch.h"
#include "cornell_box.h"

void CornellBoxScene::onLoad(ModelLoader& modelLoader)
{
	Logger::changeLogLevel(LogLevel::TRACE);

	// Model
	m_cornellBoxModel = modelLoader.loadModel("../../../Assets/Cornell-Box/CornellBox-Original.obj");

	// Cornell box 
	glm::mat4 transform    = glm::mat4(1.0f);
	transform              = glm::translate(transform, glm::vec3(0.0f, -2.0f, 0.0f));
	transform              = glm::scale(transform, glm::vec3(2.0f, 2.0f, 2.0f));

	m_cornellBox = modelLoader.createInstance(m_cornellBoxModel, transform);
}

void CornellBoxScene::onUpdate(Renderer& renderer)
{
	renderer.beginFrame();

	// Set light UBO
	renderer.ubo.lightColor     = m_lightColor;
	renderer.ubo.lightPosition  = m_lightPosition;
	renderer.ubo.lightIntensity = m_lightIntensity;

	if (renderer.isRtxEnabled())
	{
		renderer.rtxPushConstants.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		renderer.bindPipeline(Pipeline::RTX);
		renderer.bindDescriptorSets(Pipeline::RTX);
		renderer.bindRtxPushConstants();
		renderer.traceRays();
	}
	else
	{
		renderer.beginRenderPass(RenderPass::MAIN);

		renderer.setDynamicStates();

		renderer.bindPipeline(Pipeline::LIGHTING);
		renderer.bindVertexBuffer(m_cornellBoxModel.getVertexBuffer());
		renderer.bindIndexBuffer(m_cornellBoxModel.getIndexBuffer());
		renderer.bindDescriptorSets(Pipeline::LIGHTING);

		// Set push constant
		renderer.pushConstants.model    = m_cornellBox.transform;
		renderer.pushConstants.objectID = m_cornellBox.objectID;
		renderer.bindPushConstants();

		renderer.drawIndexed();

		// Visualize the light position
		glm::mat4 lightTransform     = glm::translate(glm::mat4(1.0f), m_lightPosition);
		renderer.pushConstants.model = glm::scale(lightTransform, glm::vec3(0.1f, 0.1f, 0.1f));
		renderer.bindPushConstants();
		renderer.drawIndexed();

		renderer.endRenderPass();
	}

	// Post pass
	{
		renderer.beginRenderPass(RenderPass::POST);

		renderer.setDynamicStates();

		renderer.bindPipeline(Pipeline::POST);
		renderer.bindDescriptorSets(Pipeline::POST);
		renderer.drawVertex();
		renderer.drawUI();

		renderer.endRenderPass();
	}

	renderer.submit();
	renderer.endFrame();
}

void CornellBoxScene::onUnload()
{
	m_cornellBoxModel.cleanup();
}
