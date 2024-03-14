#include "pch.h"
#include "cornell_box.h"

void CornellBoxScene::onLoad(ModelLoader& modelLoader)
{
	Logger::changeLogLevel(LogLevel::TRACE);

	// Models
	m_cornellBoxModel = modelLoader.loadModel("../../../Assets/Cornell-Box/CornellBox-Mirror.obj");
	m_mirrorModel     = modelLoader.loadModel("../../../Assets/Cube/cube_mirror.obj");

	// Cornell box 
	glm::mat4 transform1 = glm::mat4(1.0f);
	transform1           = glm::translate(transform1, glm::vec3(0.0f, -2.0f, 0.0f));
	transform1           = glm::scale(transform1, glm::vec3(2.0f, 2.0f, 2.0f));
	m_cornellBox = modelLoader.createInstance(m_cornellBoxModel, transform1);

	// Left mirror
	glm::mat4 transform2 = glm::mat4(1.0f);
	transform2           = glm::translate(transform2, glm::vec3(-7.0f, 0.0f, 0.0f));
	transform2           = glm::scale(transform2, glm::vec3(0.2f, 10.0f, 10.0f));
	m_leftMirror = modelLoader.createInstance(m_mirrorModel, transform2);

	// Right mirror
	glm::mat4 transform3 = glm::mat4(1.0f);
	transform3           = glm::translate(transform3, glm::vec3(7.0f, 0.0f, 0.0f));
	transform3           = glm::scale(transform3, glm::vec3(0.2f, 10.0f, 10.0f));
	m_rightMirror = modelLoader.createInstance(m_mirrorModel, transform3);
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
		renderer.rtxPushConstants.clearColor = { 1.0f, 1.0f, 1.0f, 1.0f };

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

		// Cornell box
		{
			renderer.pushConstants.model    = m_cornellBox.transform;
			renderer.pushConstants.objectID = m_cornellBox.objectID;
			renderer.bindPushConstants();
			renderer.drawIndexed();
		}

		// Mirrors
		{
			renderer.bindVertexBuffer(m_mirrorModel.getVertexBuffer());
			renderer.bindIndexBuffer(m_mirrorModel.getIndexBuffer());

			// Left mirror
			renderer.pushConstants.model    = m_leftMirror.transform;
			renderer.pushConstants.objectID = m_leftMirror.objectID;
			renderer.bindPushConstants();
			renderer.drawIndexed();

			// Right mirror
			renderer.pushConstants.model    = m_rightMirror.transform;
			renderer.pushConstants.objectID = m_rightMirror.objectID;
			renderer.bindPushConstants();
			renderer.drawIndexed();
		}

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
	m_mirrorModel.cleanup();
}
