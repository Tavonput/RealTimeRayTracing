#include "pch.h"
#include "example_scene.h"

void ExampleScene::onLoad(const Device& device, const CommandSystem& commandSystem)
{
	// Define two rectangles
	std::vector<Vertex> vertices = {
		// Position             Color
		{{-0.5f, -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 0.0f}},

		{{-0.4f, -0.6f,  0.0f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.6f, -0.6f,  0.0f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.6f,  0.4f,  0.0f}, {0.0f, 0.0f, 1.0f}},
		{{-0.4f,  0.4f,  0.0f}, {0.0f, 0.0f, 0.0f}}
	};

	std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	// Setup buffer creation
	Buffer::CreateInfo createInfo{};
	createInfo.device        = &device;
	createInfo.commandSystem = &commandSystem;

	// Create the vertex buffer
	createInfo.data      = vertices.data();
	createInfo.dataSize  = sizeof(Vertex) * vertices.size();
	createInfo.dataCount = static_cast<uint32_t>(vertices.size());
	m_vertexBuffer = Buffer::CreateVertexBuffer(createInfo);

	// Create the index buffer
	createInfo.data      = indices.data();
	createInfo.dataSize  = sizeof(uint32_t) * indices.size();
	createInfo.dataCount = static_cast<uint32_t>(indices.size());
	m_indexBuffer = Buffer::CreateIndexBuffer(createInfo);
}

void ExampleScene::onUpdate(RenderingContext& rctx)
{
	Renderer::BeginFrame(rctx);

	Renderer::BeginRenderPass(rctx, RenderPass::MAIN);

	Renderer::BindPipeline(rctx, Pipeline::MAIN);
	Renderer::BindVertexBuffer(rctx, m_vertexBuffer);
	Renderer::BindIndexBuffer(rctx, m_indexBuffer);
	Renderer::DrawIndexed(rctx);

	Renderer::EndRenderPass(rctx);

	Renderer::Submit(rctx);
	Renderer::EndFrame(rctx);
}

void ExampleScene::onUnload()
{
	m_vertexBuffer.cleanup();
	m_indexBuffer.cleanup();
}
