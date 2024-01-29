#include "application.h"

void Application::init(ApplicationCreateInfo& createInfo)
{
	// Logger
	m_logger.init(spdlog::level::trace);
	LOG_INFO("Logging initialization successful");

	// Context
	m_context.init(createInfo.windowHeight, createInfo.windowWidth, m_logger);

	// Swapchain initialization
	SwapchainCreateInfo swapchainCreateInfo{};
	swapchainCreateInfo.device         = &m_context.getDevice();
	swapchainCreateInfo.window         = &m_context.getWindow();
	swapchainCreateInfo.surface        = &m_context.getSurface();
	swapchainCreateInfo.framesInFlight = createInfo.framesInFlight;
	swapchainCreateInfo.logger         = m_logger;

	m_swapchain.init(swapchainCreateInfo);

	// Render pass
	m_renderPassManager.init(m_context.getDevice(), m_logger);
	createRenderPass();

	// Update the swapchain framebuffers with the render pass
	m_swapchain.setupFramebuffers(m_renderPassManager.getPass(0));

	// Command manager
	CommandManagerCreateInfo commandManagerCreateInfo{};
	commandManagerCreateInfo.device              = &m_context.getDevice();
	commandManagerCreateInfo.logger              = m_logger;
	commandManagerCreateInfo.graphicsBufferCount = createInfo.framesInFlight;

	m_commandManager.init(commandManagerCreateInfo);

	// Pipeline
	m_pipeline.init(m_context.getDevice(), m_logger, m_renderPassManager.getPass(0));
}

void Application::run()
{
	LOG_INFO("Starting main render loop");

	// Run until the window is closed
	while (!glfwWindowShouldClose(m_context.getWindow().getWindowGLFW()))
	{
		glfwPollEvents();
	}

	LOG_INFO("Main render loop ended");

	cleanup();
}

void Application::createRenderPass()
{
	LOG_INFO("Creating render pass");

	// Create render pass builder
	auto builder = RenderPass::Builder();
	builder.init(m_context.getDevice(), m_logger);

	// Add color attachment
	builder.addColorAttachment(
		m_swapchain.getFormat(),
		VK_SAMPLE_COUNT_1_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		{ {0.0f, 0.0f, 0.0f, 1.0f} });

	// Build
	auto renderPass = builder.build();

	// Store pass in manager
	m_renderPassManager.addPass(renderPass);
}

void Application::cleanup()
{
	m_commandManager.cleanup();
	m_renderPassManager.cleanup();
	m_pipeline.cleanup();
	m_swapchain.cleanup();
	m_context.cleanup();

	LOG_INFO("Application has been successfully cleaned up");
}
