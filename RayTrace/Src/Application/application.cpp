#include "pch.h"

#include "application.h"

void Application::init(ApplicationCreateInfo& createInfo)
{
	// Store frames in flight value
	m_framesInFlight = createInfo.framesInFlight;

	// Initialize logger to info level
	Logger::init(LogLevel::INFO);

	// Window
	m_window.init(createInfo.windowWidth, createInfo.windowHeight);

	// System Context
	m_context.init(m_window);

	// Swapchain initialization
	SwapchainCreateInfo swapchainCreateInfo{};
	swapchainCreateInfo.device         = &m_context.getDevice();
	swapchainCreateInfo.window         = &m_window;
	swapchainCreateInfo.surface        = &m_context.getSurface();
	swapchainCreateInfo.framesInFlight = m_framesInFlight;

	m_swapchain.init(swapchainCreateInfo);

	// Render pass manager
	m_renderPassManager.init(m_context.getDevice());
	createRenderPass();

	// Update the swapchain framebuffers with the render pass
	m_swapchain.setupFramebuffers(m_renderPassManager.getPass(0));

	// Command manager
	CommandManagerCreateInfo commandManagerCreateInfo{};
	commandManagerCreateInfo.device              = &m_context.getDevice();
	commandManagerCreateInfo.graphicsBufferCount = m_framesInFlight;

	m_commandManager.init(commandManagerCreateInfo);

	// Pipeline manager
	m_pipelineManager.init(m_context.getDevice());
	createPipeline();

	// Create scene data
	createSceneData();
}

void Application::run()
{
	// Setup rendering context
	RenderingContext rCtx(
		m_swapchain,
		m_commandManager,
		m_renderPassManager,
		m_pipelineManager,
		m_framesInFlight);

	APP_LOG_INFO("Starting main render loop");

	// Change logger to trace level
	Logger::changeLogLevel(LogLevel::TRACE);

	// Run until the window is closed
	while (!m_window.isWindowClosed())
	{
		// Process input events
		glfwPollEvents();

		{
			Renderer::BeginFrame(rCtx);

			Renderer::BeginRenderPass(rCtx, 0);

			Renderer::BindPipeline(rCtx, 0);
			Renderer::BindVertexBuffer(rCtx, m_vertexBuffer);
			Renderer::BindIndexBuffer(rCtx, m_indexBuffer);
			Renderer::DrawIndexed(rCtx, m_indexBuffer);

			Renderer::EndRenderPass(rCtx);

			Renderer::Submit(rCtx);

			Renderer::EndFrame(rCtx);
		}

	}

	// Change logger to info level
	Logger::changeLogLevel(LogLevel::INFO);

	APP_LOG_INFO("Main render loop ended");

	// Wait for the gpu to finish
	m_context.getDevice().waitForGPU();

	cleanup();
}

void Application::createRenderPass()
{
	APP_LOG_INFO("Creating render pass");

	// Create render pass builder
	auto builder = RenderPass::Builder(m_context.getDevice());

	// Add multi-sampled color attachment for MSAA
	builder.addColorAttachment(
		m_swapchain.getFormat(),
		m_swapchain.getMSAASampleCount(),
		VK_IMAGE_LAYOUT_UNDEFINED,
		{ {1.0f, 1.0f, 1.0f, 1.0f} });

	// Add depth attachment for depth buffer
	builder.addDepthAttachment(
		m_swapchain.getDepthFormat(),
		m_swapchain.getMSAASampleCount(),
		VK_IMAGE_LAYOUT_UNDEFINED,
		{ 1.0f, 0 });

	// Add resolve attachment for presentation
	builder.addResolveAttachment(
		m_swapchain.getFormat(),
		VK_IMAGE_LAYOUT_UNDEFINED,
		{ {1.0f, 1.0f, 1.0f, 1.0f} });

	// Build pass
	RenderPass renderPass = builder.buildPass();

	// Store pass in manager
	m_renderPassManager.addPass(renderPass);
}

void Application::createPipeline()
{
	APP_LOG_INFO("Creating pipeline");

	// Create a pipeline builder
	auto builder = Pipeline::Builder(m_context.getDevice());

	// Build a graphics pipeline
	Pipeline pipeline = builder.buildPipeline(
		"../../Shaders/shader_vert.spv", "../../Shaders/shader_frag.spv",
		m_renderPassManager.getPass(0),
		m_swapchain.getMSAASampleCount());

	// Store pipeline in manager
	m_pipelineManager.addPipeline(pipeline);
}

void Application::createSceneData()
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

	// Create the vertex buffer
	Buffer::CreateInfo createInfo{};
	createInfo.data           = vertices.data();
	createInfo.dataSize       = sizeof(Vertex) * vertices.size();
	createInfo.dataCount      = static_cast<uint32_t>(vertices.size());
	createInfo.device         = &m_context.getDevice();
	createInfo.commandManager = &m_commandManager;

	m_vertexBuffer = Buffer::CreateVertexBuffer(createInfo);

	// Create the index buffer
	createInfo.data      = indices.data();
	createInfo.dataSize  = sizeof(Vertex) * indices.size();
	createInfo.dataCount = static_cast<uint32_t>(indices.size());

	m_indexBuffer = Buffer::CreateIndexBuffer(createInfo);
}

void Application::cleanup()
{
	// Scene data
	m_vertexBuffer.cleanup();
	m_indexBuffer.cleanup();

	// Managers
	m_commandManager.cleanup();
	m_renderPassManager.cleanup();
	m_pipelineManager.cleanup();

	// Swapchain
	m_swapchain.cleanup();

	// System stuff
	m_context.cleanup();
	m_window.cleanup();

	APP_LOG_INFO("Application has been successfully cleaned up");
}
