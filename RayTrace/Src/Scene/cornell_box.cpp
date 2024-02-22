#include "pch.h"
#include "cornell_box.h"

void CornellBoxScene::onLoad(ModelLoader& modelLoader)
{
	Logger::changeLogLevel(LogLevel::TRACE);

	// Model
	m_cornellBoxModel = modelLoader.loadModel("../../../Assets/Cornell-Box/CornellBox-Original.obj");

	// Cornell box instance
	m_cornellBox = m_cornellBoxModel.createInstance();

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3(0.0f, -2.0f, 0.0f));
	transform = glm::scale(transform, glm::vec3(2.0f, 2.0f, 2.0f));
	m_cornellBox.transform = transform;
}

void CornellBoxScene::onUpdate(Renderer& renderer)
{
	renderer.beginFrame();
	renderer.beginRenderPass(RenderPass::MAIN);

	renderer.bindPipeline(Pipeline::LIGHTING);
	renderer.bindVertexBuffer(m_cornellBoxModel.getVertexBuffer());
	renderer.bindIndexBuffer(m_cornellBoxModel.getIndexBuffer());
	renderer.bindDescriptorSets();

	// Set light UBO
	renderer.ubo.lightColor     = m_lightColor;
	renderer.ubo.lightPosition  = m_lightPosition;
	renderer.ubo.lightIntensity = m_lightIntensity;

	// Set push constant
	renderer.pushConstants.model    = m_cornellBox.transform;
	renderer.pushConstants.objectID = m_cornellBox.objectID;
	renderer.bindPushConstants();

	renderer.drawIndexed();

	 // Visualize the light position
	 // glm::mat4 lightTransform = glm::translate(glm::mat4(1.0f), m_lightPosition);
	 // renderer.pushConstants.model = glm::scale(lightTransform, glm::vec3(0.1f, 0.1f, 0.1f));
	 // renderer.bindPushConstants();
	 // renderer.drawIndexed();

	renderer.endRenderPass();
	renderer.submit();
	renderer.endFrame();
}

void CornellBoxScene::onUnload()
{
	m_cornellBoxModel.cleanup();
}
