#include "pch.h"
#include "simple_cube_scene.h"

void SimpleCubeScene::onLoad(const Device& device, const CommandSystem& commandSystem)
{
	// Cube data
	std::vector<Vertex> vertices = {
		// Red
		{{ 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{-0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},

		// Teal
		{{-0.5f, -0.5f,  0.5f},  {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f},  {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},

		// Magenta
		{{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f},  {1.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},

		// Yellow
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},

		// Green
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},

		// Blue
		{{ 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
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

void SimpleCubeScene::onUpdate(RenderingContext& rctx)
{
	Renderer::BeginFrame(rctx);
	Renderer::BeginRenderPass(rctx, RenderPass::MAIN);

	// Bind common resources
	Renderer::BindVertexBuffer(rctx, m_vertexBuffer);
	Renderer::BindDescriptorSets(rctx);
	
	// Draw main spinning cube
	Renderer::BindPipeline(rctx, Pipeline::LIGHTING);
	updateMainCubeTransform(rctx);
	Renderer::BindPushConstants(rctx);
	Renderer::DrawVertex(rctx);

	// Draw light cube
	Renderer::BindPipeline(rctx, Pipeline::FLAT);
	updateLightPosition(rctx);
	Renderer::BindPushConstants(rctx);
	Renderer::DrawVertex(rctx);

	Renderer::EndRenderPass(rctx);
	Renderer::Submit(rctx);
	Renderer::EndFrame(rctx);
}

void SimpleCubeScene::onUnload()
{
	m_vertexBuffer.cleanup();
}

void SimpleCubeScene::updateMainCubeTransform(RenderingContext& rctx)
{
	// Rotations
	m_mainCubeTransform = glm::rotate(m_mainCubeTransform, rctx.deltaTime * glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X-axis
	m_mainCubeTransform = glm::rotate(m_mainCubeTransform, rctx.deltaTime * glm::radians(90.0f),  glm::vec3(0.0f, 1.0f, 0.0f)); // Y-axis
	m_mainCubeTransform = glm::rotate(m_mainCubeTransform, rctx.deltaTime * glm::radians(45.0f),  glm::vec3(0.0f, 0.0f, 1.0f)); // Z-axis

	// Update push constant model matrix
	rctx.pushConstants.model = m_mainCubeTransform;
}

void SimpleCubeScene::updateLightPosition(RenderingContext& rctx)
{
	// Compute new position
	float angleDelta = rctx.deltaTime * m_speed;
	m_totalAngle += angleDelta;

	m_currentPosition.x = m_radius * cos(m_totalAngle);
	m_currentPosition.y = m_radius * sin(m_totalAngle);
	m_currentPosition.z = m_radius * sin(m_totalAngle);

	// Set UBO light position
	rctx.ubo.lightPosition = m_currentPosition;

	// Update model matrix
	m_lightCubeTransform = glm::translate(glm::mat4(1.0f), m_currentPosition);
	m_lightCubeTransform = glm::scale(m_lightCubeTransform, glm::vec3(0.1f, 0.1f, 0.1f));

	// Update push constant model matrix
	rctx.pushConstants.model = m_lightCubeTransform;
}
