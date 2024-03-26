#include "pch.h"
#include "model_viewer.h"

void ModelViewerScene::onLoad(SceneBuilder& scene)
{
	// Models
	std::string modelPath = "../../../../Other-Assets/fireplace_room/fireplace_room.obj";
	m_mainModel = scene.loadModel(modelPath);

	glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale       = { 1.0f, 1.0f, 1.0f };

	// Create instance
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, translation);
	transform = glm::scale(transform, scale);
	m_model   = scene.createInstance(m_mainModel, transform);

	// Set initial conditions
	scene.setLightPosition(glm::vec3(0.0f, 2.0f, 0.0f));
	scene.setBackgroundColor(glm::vec3(1.0f, 1.0f, 1.0f));

	// Set custom UI check boxes
	scene.addUICheckBox("Visualize Light (Raster Only)", &m_visualizeLight);
}

void ModelViewerScene::onUpdate(Renderer& renderer)
{
	renderer.beginFrame();

	if (renderer.isRtxEnabled())
	{
		renderer.bindRtxPipeline();
		renderer.bindRtxDescriptorSets();
		renderer.bindRtxPushConstants();
		renderer.traceRays();
	}
	else // Render rasterized scene
	{
		renderer.beginRenderPass(RenderPass::MAIN);

		renderer.setDynamicStates();

		// Model
		{
			renderer.bindPipeline(Pipeline::LIGHTING);
			renderer.bindVertexBuffer(m_mainModel.getVertexBuffer());
			renderer.bindIndexBuffer(m_mainModel.getIndexBuffer());
			renderer.bindDescriptorSets(Pipeline::LIGHTING);

			renderer.pushConstants.model    = m_model.transform;
			renderer.pushConstants.objectID = m_model.objectID;
			renderer.bindPushConstants(Pipeline::LIGHTING);
			renderer.drawIndexed();
		}

		if (m_visualizeLight)
		{
			renderer.bindPipeline(Pipeline::FLAT);

			glm::mat4 lightTransform           = glm::translate(glm::mat4(1.0f), renderer.ubo.lightPosition);
			renderer.pushConstants.model       = glm::scale(lightTransform, glm::vec3(0.1f, 0.1f, 0.1f));
			renderer.pushConstants.objectColor = renderer.ubo.lightColor;
			renderer.bindPushConstants(Pipeline::FLAT);

			renderer.drawIndexed();
		}

		renderer.endRenderPass();
	}

	// Post pass
	{
		renderer.beginRenderPass(RenderPass::POST);

		renderer.setDynamicStates();

		renderer.bindPipeline(Pipeline::POST);
		renderer.bindDescriptorSets(Pipeline::POST);
		renderer.bindPushConstants(Pipeline::POST);
		renderer.drawVertex();
		renderer.drawUI();

		renderer.endRenderPass();
	}

	renderer.submit();
	renderer.endFrame();
}

void ModelViewerScene::onUnload()
{
	m_mainModel.cleanup();
}
