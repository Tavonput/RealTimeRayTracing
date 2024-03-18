#include "pch.h"
#include "dragon.h"

void DragonScene::onLoad(SceneBuilder& scene)
{
	// Models
	m_dragonModel = scene.loadModel("../../../Assets/Dragon/dragon.obj");
	m_planeModel  = scene.loadModel("../../../Assets/Plane/plane.obj");
	m_lightModel = scene.loadModel("../../../Assets/Cube/cube_light.obj");

	// Dragon 
	 glm::mat4 dragonTrans = glm::mat4(1.0f);
	 dragonTrans           = glm::scale(dragonTrans, glm::vec3(2.0f, 2.0f, 2.0f));
	 dragonTrans           = glm::rotate(dragonTrans, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	 dragonTrans           = glm::translate(dragonTrans, glm::vec3(0.0f, 0.25f, 0.0f));
	 m_dragon = scene.createInstance(m_dragonModel, dragonTrans);

	// Plane
	glm::mat4 planTrans = glm::mat4(1.0f);
	planTrans           = glm::scale(planTrans, glm::vec3(2.0f, 2.0f, 2.0f));
	m_plane = scene.createInstance(m_planeModel, planTrans);

	// Light
	glm::mat4 lightTrans = glm::mat4(1.0f);
	lightTrans           = glm::translate(lightTrans, glm::vec3(3.0f, 1.0f, 0.0f));
	lightTrans           = glm::scale(lightTrans, glm::vec3(0.05f, 2.0f, 2.0f));
	m_light = scene.createInstance(m_lightModel, lightTrans);

	// Set initial conditions
	scene.setLightPosition(glm::vec3(2.9f, 1.0f, 0.0f));
	scene.setBackgroundColor(glm::vec3(0.0f, 0.0f, 0.0f));

	// Set custom UI check boxes
	scene.addUICheckBox("Visualize Light (Raster Only)", &m_visualizeLight);
}

void DragonScene::onUpdate(Renderer& renderer)
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

		// Dragon
		{
			renderer.bindPipeline(Pipeline::LIGHTING);
			renderer.bindVertexBuffer(m_dragonModel.getVertexBuffer());
			renderer.bindIndexBuffer(m_dragonModel.getIndexBuffer());
			renderer.bindDescriptorSets(Pipeline::LIGHTING);

			renderer.pushConstants.model    = m_dragon.transform;
			renderer.pushConstants.objectID = m_dragon.objectID;
			renderer.bindPushConstants(Pipeline::LIGHTING);
			renderer.drawIndexed();
		}

		// Plane
		{
			renderer.bindPipeline(Pipeline::LIGHTING);
			renderer.bindVertexBuffer(m_planeModel.getVertexBuffer());
			renderer.bindIndexBuffer(m_planeModel.getIndexBuffer());
			renderer.bindDescriptorSets(Pipeline::LIGHTING);

			renderer.pushConstants.model    = m_plane.transform;
			renderer.pushConstants.objectID = m_plane.objectID;
			renderer.bindPushConstants(Pipeline::LIGHTING);
			renderer.drawIndexed();
		}

		// Light
		{
			renderer.bindPipeline(Pipeline::FLAT);
			renderer.bindVertexBuffer(m_lightModel .getVertexBuffer());
			renderer.bindIndexBuffer(m_lightModel.getIndexBuffer());
			renderer.bindDescriptorSets(Pipeline::FLAT);

			renderer.pushConstants.model       = m_light.transform;
			renderer.pushConstants.objectColor = renderer.ubo.lightColor;
			renderer.pushConstants.objectID    = m_light.objectID;
			renderer.bindPushConstants(Pipeline::FLAT);
			renderer.drawIndexed();
		}

		if (m_visualizeLight)
		{
			renderer.bindPipeline(Pipeline::FLAT);
			renderer.bindVertexBuffer(m_lightModel.getVertexBuffer());
			renderer.bindIndexBuffer(m_lightModel.getIndexBuffer());

			glm::mat4 lightTransform           = glm::translate(glm::mat4(1.0f), renderer.ubo.lightPosition);
			renderer.pushConstants.model       = glm::scale(lightTransform, glm::vec3(0.01f, 0.01f, 0.01f));
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

void DragonScene::onUnload()
{
	m_dragonModel.cleanup();
	m_planeModel.cleanup();
	m_lightModel.cleanup();
}
