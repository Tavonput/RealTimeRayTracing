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

	// Initialize push constant stuff
	m_model = glm::mat4(1.0f);
}

void SimpleCubeScene::onUpdate(RenderingContext& rctx)
{
	Renderer::BeginFrame(rctx);

	updateRenderMatrix(rctx.deltaTime, rctx.aspectRatio);

	Renderer::BeginRenderPass(rctx, RenderPass::MAIN);

	Renderer::BindPipeline(rctx, Pipeline::MAIN);
	Renderer::BindVertexBuffer(rctx, m_vertexBuffer);
	Renderer::PushConstants(rctx, m_cameraMatrix);
	Renderer::DrawVertex(rctx);

	Renderer::EndRenderPass(rctx);

	Renderer::Submit(rctx);
	Renderer::EndFrame(rctx);
}

void SimpleCubeScene::onUnload()
{
	m_vertexBuffer.cleanup();
}

void SimpleCubeScene::updateRenderMatrix(float time, float aspectRatio)
{
	m_model = glm::rotate(m_model, time * glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	m_model = glm::rotate(m_model, time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_model = glm::rotate(m_model, time * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 view = glm::lookAt(m_cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	projection[1][1] *= -1;

	m_cameraMatrix.renderMatrix = projection * view * m_model;
	m_cameraMatrix.model = m_model;
}
