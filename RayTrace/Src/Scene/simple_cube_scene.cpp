#include "pch.h"
#include "simple_cube_scene.h"

void SimpleCubeScene::onLoad(const Device& device, const CommandSystem& commandSystem)
{
	// Cube data
	std::vector<Vertex> vertices = {
		// Red
		{{ 0.5f, -0.5f, -0.5f},  {1.0f, 0.1f, 0.1f}, {0.0f, 0.0f, -1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {1.0f, 0.1f, 0.1f}, {0.0f, 0.0f, -1.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 0.1f, 0.1f}, {0.0f, 0.0f, -1.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 0.1f, 0.1f}, {0.0f, 0.0f, -1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {1.0f, 0.1f, 0.1f}, {0.0f, 0.0f, -1.0f}},
		{{-0.5f,  0.5f, -0.5f},  {1.0f, 0.1f, 0.1f}, {0.0f, 0.0f, -1.0f}},

		// Teal
		{{-0.5f, -0.5f,  0.5f},  {0.1f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {0.1f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {0.1f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {0.1f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f},  {0.1f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f},  {0.1f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},

		// Magenta
		{{-0.5f,  0.5f,  0.5f},  {1.0f, 0.1f, 0.6f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {1.0f, 0.1f, 0.6f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f},  {1.0f, 0.1f, 0.6f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f},  {1.0f, 0.1f, 0.6f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f},  {1.0f, 0.1f, 0.6f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},  {1.0f, 0.1f, 0.6f}, {-1.0f, 0.0f, 0.0f}},

		// Yellow
		{{ 0.5f,  0.5f, -0.5f},  {0.9f, 1.0f, 0.1f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {0.9f, 1.0f, 0.1f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {0.9f, 1.0f, 0.1f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {0.9f, 1.0f, 0.1f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {0.9f, 1.0f, 0.1f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {0.9f, 1.0f, 0.1f}, {1.0f, 0.0f, 0.0f}},

		// Green
		{{-0.5f, -0.5f, -0.5f},  {0.1f, 1.0f, 0.1f}, {0.0f, -1.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {0.1f, 1.0f, 0.1f}, {0.0f, -1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {0.1f, 1.0f, 0.1f}, {0.0f, -1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {0.1f, 1.0f, 0.1f}, {0.0f, -1.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f},  {0.1f, 1.0f, 0.1f}, {0.0f, -1.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.1f, 1.0f, 0.1f}, {0.0f, -1.0f, 0.0f}},

		// Blue
		{{ 0.5f,  0.5f, -0.5f},  {0.1f, 0.1f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {0.1f, 0.1f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {0.1f, 0.1f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {0.1f, 0.1f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {0.1f, 0.1f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},  {0.1f, 0.1f, 1.0f}, {0.0f, 1.0f, 0.0f}},
	};

	// Create vertex buffer
	Buffer::CreateInfo createInfo{};
	createInfo.device        = &device;
	createInfo.commandSystem = &commandSystem;
	createInfo.data          = vertices.data();
	createInfo.dataSize      = sizeof(Vertex) * vertices.size();
	createInfo.dataCount     = static_cast<uint32_t>(vertices.size());
	m_vertexBuffer = Buffer::CreateVertexBuffer(createInfo);

	// Set up transforms
	m_mainCubeTransform = glm::mat4(1.0f);
	m_mainCubeTransform = glm::scale(m_mainCubeTransform, glm::vec3(2.0f, 2.0f, 2.0f));

	m_lightCubeTransform = glm::mat4(1.0f);
}

void SimpleCubeScene::onUpdate(Renderer& renderer)
{
	renderer.beginFrame();
	renderer.beginRenderPass(RenderPass::MAIN);

	// Bind common resources
	renderer.bindVertexBuffer(m_vertexBuffer);
	renderer.bindDescriptorSets();
	
	// Draw main spinning cube
	renderer.bindPipeline(Pipeline::LIGHTING);
	renderer.pushConstants.model = m_mainCubeTransform;
	renderer.bindPushConstants();
	renderer.drawVertex();

	// Draw light cube
	renderer.bindPipeline(Pipeline::FLAT);
	updateLightPosition(renderer);
	renderer.bindPushConstants();
	renderer.drawVertex();

	renderer.endRenderPass();
	renderer.submit();
	renderer.endFrame();
}

void SimpleCubeScene::onUnload()
{
	m_vertexBuffer.cleanup();
}

void SimpleCubeScene::updateLightPosition(Renderer& renderer)
{
	// Compute new position
	float angleDelta = renderer.deltaTime * m_speed;
	m_totalAngle += angleDelta;

	m_lightPosition.x = m_radius * cos(m_totalAngle * 0.5f);
	m_lightPosition.y = m_radius * sin(m_totalAngle * 0.8f);
	m_lightPosition.z = m_radius * sin(m_totalAngle * 0.3f);

	// Set UBO light position
	renderer.ubo.lightPosition = m_lightPosition;

	// Update model matrix
	m_lightCubeTransform = glm::translate(glm::mat4(1.0f), m_lightPosition);
	m_lightCubeTransform = glm::scale(m_lightCubeTransform, glm::vec3(0.1f, 0.1f, 0.1f));

	// Update push constants
	renderer.pushConstants.model = m_lightCubeTransform;
	renderer.pushConstants.objectColor = m_lightColor;

	// Also set light color
	renderer.ubo.lightColor = m_lightColor;
}
