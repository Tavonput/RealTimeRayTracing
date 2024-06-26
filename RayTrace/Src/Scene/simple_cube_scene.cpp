#include "pch.h"
#include "simple_cube_scene.h"

void SimpleCubeScene::onLoad(SceneBuilder& scene)
{
	// Cube model
	m_cubeModel = scene.loadModel("../../../Assets/Cube/cube.obj");

	// Cube instances
	glm::mat4 mainCubeTransform = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
	m_mainCube = scene.createInstance(m_cubeModel, mainCubeTransform);

	glm::mat4 lightCubeTransform = glm::mat4(1.0f);
	m_lightCube = scene.createInstance(m_cubeModel, lightCubeTransform);

	scene.setLightPosition(m_lightPosition);
}

void SimpleCubeScene::onUpdate(Renderer& renderer)
{
	renderer.beginFrame();

	if (renderer.isRtxEnabled())
	{
		renderer.rtxPushConstants.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		updateLightCube(renderer);

		renderer.bindPipeline(Pipeline::RTX_RT);
		renderer.bindDescriptorSets(Pipeline::RTX_RT);
		renderer.bindRtxPushConstants();
		renderer.traceRays();
	}
	else
	{
		renderer.beginRenderPass(RenderPass::MAIN);

		renderer.setDynamicStates();

		// Bind buffers
		renderer.bindVertexBuffer(m_cubeModel.getVertexBuffer());
		renderer.bindIndexBuffer(m_cubeModel.getIndexBuffer());

		// Draw main cube
		renderer.bindPipeline(Pipeline::LIGHTING);
		renderer.bindDescriptorSets(Pipeline::LIGHTING);

		renderer.pushConstants.model = m_mainCube.transform;
		renderer.pushConstants.objectID = m_mainCube.objectID;
		renderer.bindPushConstants(Pipeline::LIGHTING);
		renderer.drawIndexed();


		// Draw light cube
		renderer.bindPipeline(Pipeline::FLAT);
		updateLightCube(renderer);
		renderer.bindPushConstants(Pipeline::FLAT);
		renderer.drawIndexed();

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

void SimpleCubeScene::onUnload()
{
	m_cubeModel.cleanup();
}

void SimpleCubeScene::updateLightCube(Renderer& renderer)
{
	// Compute new position
	float angleDelta = renderer.deltaTime * m_speed;
	m_totalAngle += angleDelta;

	m_lightPosition.x = m_radius * cos(m_totalAngle * 0.5f);
	m_lightPosition.y = m_radius * sin(m_totalAngle * 0.8f);
	m_lightPosition.z = m_radius * sin(m_totalAngle * 0.3f);

	// Update model matrix
	m_lightCube.transform = glm::translate(glm::mat4(1.0f), m_lightPosition);
	m_lightCube.transform = glm::scale(m_lightCube.transform, glm::vec3(0.1f, 0.1f, 0.1f));

	// Set UBO light
	renderer.ubo.lightPosition = m_lightPosition;
	renderer.ubo.lightColor    = m_lightColor;

	// Update push constants
	renderer.pushConstants.model       = m_lightCube.transform;
	renderer.pushConstants.objectColor = m_lightColor;
	renderer.pushConstants.objectID    = m_lightCube.objectID;
}
