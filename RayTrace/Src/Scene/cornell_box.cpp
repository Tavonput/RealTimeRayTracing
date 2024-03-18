#include "pch.h"
#include "cornell_box.h"

void CornellBoxScene::onLoad(SceneBuilder& scene)
{
	// Models
	m_cornellBoxModel = scene.loadModel("../../../Assets/Cornell-Box/CornellBox-PBR.obj");
	m_mirrorModel     = scene.loadModel("../../../Assets/Cube/cube_mirror.obj");
	m_planeModel      = scene.loadModel("../../../Assets/Plane/plane.obj");
	
	// Cornell box 
	glm::mat4 cornellTrans = glm::mat4(1.0f);
	cornellTrans           = glm::translate(cornellTrans, glm::vec3(0.0f, -2.0f, 0.0f));
	cornellTrans           = glm::scale(cornellTrans, glm::vec3(2.0f, 2.0f, 2.0f));
	m_cornellBox = scene.createInstance(m_cornellBoxModel, cornellTrans);

	// Left mirror
	glm::mat4 leftMirrorTrans = glm::mat4(1.0f);
	leftMirrorTrans           = glm::translate(leftMirrorTrans, glm::vec3(-7.0f, 0.0f, 0.0f));
	leftMirrorTrans           = glm::scale(leftMirrorTrans, glm::vec3(0.2f, 10.0f, 10.0f));
	m_leftMirror = scene.createInstance(m_mirrorModel, leftMirrorTrans);

	// Right mirror
	glm::mat4 rightMirrorTrans = glm::mat4(1.0f);
	rightMirrorTrans           = glm::translate(rightMirrorTrans, glm::vec3(7.0f, 0.0f, 0.0f));
	rightMirrorTrans           = glm::scale(rightMirrorTrans, glm::vec3(0.2f, 10.0f, 10.0f));
	m_rightMirror = scene.createInstance(m_mirrorModel, rightMirrorTrans);

	// Plane
	glm::mat4 planTrans = glm::mat4(1.0f);
	planTrans           = glm::translate(planTrans, glm::vec3(0.0f, -2.05f, 0.0f));
	planTrans           = glm::scale(planTrans, glm::vec3(10.0f, 10.0f, 10.0f));
	m_plane = scene.createInstance(m_planeModel, planTrans);

	// Set initial conditions
	scene.setLightPosition(glm::vec3(0.0f, 1.8f, 0.0f));
	scene.setBackgroundColor(glm::vec3(0.0f, 0.0f, 0.0f));

	// Set custom UI check boxes
	scene.addUICheckBox("Enable Mirrors (Raster Only)", &m_renderMirrors);
	scene.addUICheckBox("Visualize Light (Raster Only)", &m_visualizeLight);
}

void CornellBoxScene::onUpdate(Renderer& renderer)
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



		// Cornell box
		{
			renderer.bindPipeline(Pipeline::LIGHTING);
			renderer.bindVertexBuffer(m_cornellBoxModel.getVertexBuffer());
			renderer.bindIndexBuffer(m_cornellBoxModel.getIndexBuffer());
			renderer.bindDescriptorSets(Pipeline::LIGHTING);

			renderer.pushConstants.model    = m_cornellBox.transform;
			renderer.pushConstants.objectID = m_cornellBox.objectID;
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
			renderer.drawVertex();
		}

		// Mirrors
		if (m_renderMirrors)
		{
			renderer.bindVertexBuffer(m_mirrorModel.getVertexBuffer());
			renderer.bindIndexBuffer(m_mirrorModel.getIndexBuffer());

			// Left mirror
			renderer.pushConstants.model    = m_leftMirror.transform;
			renderer.pushConstants.objectID = m_leftMirror.objectID;
			renderer.bindPushConstants(Pipeline::LIGHTING);
			renderer.drawIndexed();

			// Right mirror
			renderer.pushConstants.model    = m_rightMirror.transform;
			renderer.pushConstants.objectID = m_rightMirror.objectID;
			renderer.bindPushConstants(Pipeline::LIGHTING);
			renderer.drawIndexed();
		}

		// Light cube
		if (m_visualizeLight)
		{
			renderer.bindPipeline(Pipeline::FLAT);
			renderer.bindVertexBuffer(m_mirrorModel.getVertexBuffer());
			renderer.bindIndexBuffer(m_mirrorModel.getIndexBuffer());

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

void CornellBoxScene::onUnload()
{
	m_cornellBoxModel.cleanup();
	m_mirrorModel.cleanup();
	m_planeModel.cleanup();
}
