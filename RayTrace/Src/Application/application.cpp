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

	// Render pass
	createRenderPass();

	// Update the swapchain framebuffers with the render pass
	m_swapchain.setupFramebuffers(m_renderPasses[RenderPass::MAIN].renderPass);

	// Command system
	m_commandSystem.init(m_context.getDevice(), m_framesInFlight);

	// Pipeline
	createPipeline();
}

void Application::run()
{
	// Setup rendering context
	RenderingContext rctx(
		m_swapchain,
		m_commandSystem,
		m_renderPasses,
		m_pipelines,
		m_framesInFlight);

	// Load scene
	APP_LOG_INFO("Loading scene");
	SimpleCubeScene scene;
	scene.onLoad(m_context.getDevice(), m_commandSystem);

	Logger::changeLogLevel(LogLevel::TRACE);
	APP_LOG_INFO("Starting main render loop");

	// Run until the window is closed
	while (!m_window.isWindowClosed())
	{
		glfwPollEvents();
		scene.onUpdate(rctx);
	}

	APP_LOG_INFO("Main render loop ended");
	Logger::changeLogLevel(LogLevel::INFO);

	// Cleanup
	m_context.getDevice().waitForGPU();
	scene.onUnload();
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
		{ {0.0f, 0.0f, 0.0f, 1.0f} });

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
		{ {0.0f, 0.0f, 0.0f, 1.0f} });

	// Build pass - MAIN
	m_renderPasses.push_back(builder.buildPass());
}

void Application::createPipeline()
{
	APP_LOG_INFO("Creating pipeline");

	// Create a pipeline builder
	auto builder = Pipeline::Builder(m_context.getDevice());

	// Build a graphics pipeline - MAIN
	m_pipelines.push_back(
		builder.buildPipeline(
			"../../Shaders/shader_vert.spv", "../../Shaders/shader_frag.spv",
			m_renderPasses[RenderPass::MAIN].renderPass,
			m_swapchain.getMSAASampleCount())
	);
}

void Application::cleanup()
{
	// Render passes
	for (auto& renderPass : m_renderPasses)
		renderPass.cleanup(m_context.getDevice());

	// Pipelines
	for (auto& pipeline : m_pipelines)
		pipeline.cleanup(m_context.getDevice());

	// Command System
	m_commandSystem.cleanup();
	
	// Swapchain
	m_swapchain.cleanup();

	// System stuff
	m_context.cleanup();
	m_window.cleanup();

	APP_LOG_INFO("Application has been successfully cleaned up");
}
