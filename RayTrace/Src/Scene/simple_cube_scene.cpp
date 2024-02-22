#include "pch.h"
#include "simple_cube_scene.h"

void SimpleCubeScene::onLoad(ModelLoader& modelLoader)
{
	// Cube model
	m_cubeModel = modelLoader.loadModel("../../../Assets/Cube/cube.obj");

	// Cube instances
	m_mainCube = m_cubeModel.createInstance();
	m_mainCube.transform = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));

	m_lightCube = m_cubeModel.createInstance();
	m_lightCube.transform = glm::mat4(1.0f);
}

void SimpleCubeScene::onUpdate(Renderer& renderer)
{
	renderer.beginFrame();
	renderer.beginRenderPass(RenderPass::MAIN);

	// Bind common resources
	renderer.bindVertexBuffer(m_cubeModel.getVertexBuffer());
	renderer.bindIndexBuffer(m_cubeModel.getIndexBuffer());
	renderer.bindDescriptorSets();
	
	// Draw main cube
	renderer.bindPipeline(Pipeline::LIGHTING);

	renderer.pushConstants.model = m_mainCube.transform;
	renderer.pushConstants.objectID = m_mainCube.objectID;
	renderer.bindPushConstants();

	renderer.drawIndexed();

	// Draw light cube
	renderer.bindPipeline(Pipeline::FLAT);

	updateLightCube(renderer);
	renderer.bindPushConstants();

	renderer.drawIndexed();

	renderer.endRenderPass();
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
