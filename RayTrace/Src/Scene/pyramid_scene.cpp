#include "pch.h"
#include "pyramid_scene.h"


void PyramidScene::onLoad(const Device& device, const CommandSystem& commandSystem)
{
	
	

	std::vector<Vertex> vertices{
		
		//Vertex Coordinates      Color (R, G, B)     Normal Vector
		//Front face
		{{-0.5f, -0.5f,  0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f,  1.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f,  1.0f}},
		{{ 0.0f,  0.5f,  0.0f},  {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f,  1.0f}},

		//Left face
		{{0.0f,   0.5f,  0.0f},  {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f,  0.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f,  0.0f}},
		{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f,  0.0f}},

		//Back face
		{{ 0.0f,  0.5f,  0.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f,  -1.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f,  -1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f,  -1.0f}},

		//Right face
		{{ 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f,   0.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f,   0.0f}},
		{{ 0.0f,  0.5f,  0.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f,   0.0f}},

	};  //Vertices must be drawn in counterclockwise order relative to front face to avoid unwanted backface culling***

	// Create vertex buffer
	Buffer::CreateInfo createInfo{};
	createInfo.device = &device;
	createInfo.commandSystem = &commandSystem;
	createInfo.data = vertices.data();
	createInfo.dataSize = sizeof(Vertex) * vertices.size();
	createInfo.dataCount = static_cast<uint32_t>(vertices.size());

	m_vertexBuffer = Buffer::CreateVertexBuffer(createInfo);

	// Initialize push constant stuff
	m_model = glm::mat4(1.0f); //Matrix 
}

void PyramidScene::onUpdate(RenderingContext& rctx)
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

void PyramidScene::onUnload()
{
	m_vertexBuffer.cleanup();
}

void PyramidScene::updateRenderMatrix(float time, float aspectRatio)
{
	
	
	//rotate(Model Matrix, Degrees of rotation, axis of rotation)
	m_model = glm::rotate(m_model, time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	

	//Transformation from world coordinates to view coordinates
	//lookAt(Camera coordinates, Where camera is looking, The "up" direction in the world)
	glm::mat4 view = glm::lookAt(m_cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 

	//Projection transformation to give scene perspective
	//perspective(FOV, aspect, near, far)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	projection[1][1] *= -1;

	m_cameraMatrix.renderMatrix = projection * view * m_model;
	m_cameraMatrix.model = m_model;
}
