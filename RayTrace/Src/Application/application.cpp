#include "pch.h"

#include "application.h"

void Application::init(ApplicationCreateInfo& createInfo)
{
	// Store frames in flight value
	m_framesInFlight = createInfo.framesInFlight;

	// Logger
	m_logger.init(spdlog::level::trace);
	LOG_INFO("Logging initialization successful");

	// Window
	m_window.init(createInfo.windowWidth, createInfo.windowHeight, m_logger);

	// System Context
	m_context.init(m_window, m_logger);

	// Swapchain initialization
	SwapchainCreateInfo swapchainCreateInfo{};
	swapchainCreateInfo.device         = &m_context.getDevice();
	swapchainCreateInfo.window         = &m_window;
	swapchainCreateInfo.surface        = &m_context.getSurface();
	swapchainCreateInfo.framesInFlight = m_framesInFlight;
	swapchainCreateInfo.logger         = m_logger;

	m_swapchain.init(swapchainCreateInfo);

	// Render pass manager
	m_renderPassManager.init(m_context.getDevice(), m_logger);
	createRenderPass();

	// Update the swapchain framebuffers with the render pass
	m_swapchain.setupFramebuffers(m_renderPassManager.getPass(0));

	// Command manager
	CommandManagerCreateInfo commandManagerCreateInfo{};
	commandManagerCreateInfo.device              = &m_context.getDevice();
	commandManagerCreateInfo.logger              = m_logger;
	commandManagerCreateInfo.graphicsBufferCount = m_framesInFlight;

	m_commandManager.init(commandManagerCreateInfo);

	// Pipeline manager
	m_pipelineManager.init(m_context.getDevice(), m_logger);
	createPipeline();

	// Create vertex buffer
	createVertexBuffer();
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

	LOG_INFO("Starting main render loop");

	// Run until the window is closed
	while (!glfwWindowShouldClose(m_window.getWindowGLFW()))
	{
		// Process input events
		glfwPollEvents();

		{
			Renderer::BeginFrame(rCtx);

			Renderer::BeginRenderPass(rCtx, 0);

			Renderer::BindPipeline(rCtx, 0);
			Renderer::DrawVertex(rCtx, m_vertexBuffer);

			Renderer::EndRenderPass(rCtx);

			Renderer::Submit(rCtx);

			Renderer::EndFrame(rCtx);
		}

	}

	LOG_INFO("Main render loop ended");

	// Wait for the gpu to finish
	vkDeviceWaitIdle(m_context.getDevice().getLogical());

	cleanup();
}

void Application::createRenderPass()
{
	LOG_INFO("Creating render pass");

	// Create render pass builder
	auto builder = RenderPass::Builder(m_context.getDevice(), m_logger);

	// Add color attachment
	builder.addColorAttachment(
		m_swapchain.getFormat(),      // Format
		VK_SAMPLE_COUNT_1_BIT,        // Samples per pixel
		VK_IMAGE_LAYOUT_UNDEFINED,    // Initial layout
		{ {0.0f, 0.0f, 0.0f, 1.0f} }, // Clear color
		true);                        // Use for presentation

	// Build pass
	RenderPass renderPass = builder.buildPass();

	// Store pass in manager
	m_renderPassManager.addPass(renderPass);
}

void Application::createPipeline()
{
	LOG_INFO("Creating pipeline");

	// Create a pipeline builder
	auto builder = Pipeline::Builder(m_context.getDevice(), m_logger);

	// Build a graphics pipeline
	Pipeline pipeline = builder.buildPipeline(
		"../../Shaders/shader_vert.spv", "../../Shaders/shader_frag.spv", // Shaders
		m_renderPassManager.getPass(0));                                  // Render pass

	// Store pipeline in manager
	m_pipelineManager.addPipeline(pipeline);
}

void Application::createVertexBuffer()
{
	// Define a triangle
	std::vector<Vertex> vertices = {
		// Position            Color
		{{ 0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}}
	};

	// Create the vertex buffer
	m_vertexBuffer = Buffer(
		BufferType::VERTEX,                     // Type
		vertices.data(),                        // Data
		sizeof(Vertex) * vertices.size(),       // Total bytes
		static_cast<uint32_t>(vertices.size()), // Number of vertices
		m_context.getDevice(),                  // Device
		m_commandManager,                       // Command manager
		m_logger);                              // Logger
}

void Application::cleanup()
{
	// Scene data
	m_vertexBuffer.cleanup();

	// Managers
	m_commandManager.cleanup();
	m_renderPassManager.cleanup();
	m_pipelineManager.cleanup();

	// Swapchain
	m_swapchain.cleanup();

	// System stuff
	m_context.cleanup();
	m_window.cleanup();

	LOG_INFO("Application has been successfully cleaned up");
}
