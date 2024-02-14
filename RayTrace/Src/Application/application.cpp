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

	// Uniform buffers
	Buffer::CreateInfo uboInfo{};
	uboInfo.device        = &m_context.getDevice();
	uboInfo.commandSystem = &m_commandSystem;
	uboInfo.dataSize      = sizeof(GlobalUniform);

	for (uint8_t i = 0; i < m_framesInFlight; i++)
		m_uniformBuffers.push_back(Buffer::CreateUniformBuffer(uboInfo));

	// Descriptor Sets
	createDescriptorSets();

	// Camera
	Camera::CreateInfo cameraInfo{};
	cameraInfo.position  = { 4.0f, 4.0f, 4.0f };
	cameraInfo.fov       = 45.0f;
	cameraInfo.nearPlane = 0.1f;
	cameraInfo.farPlane  = 100.0f;

	m_camera.init(cameraInfo);

	// Pipeline
	createPipelines();
}

void Application::run()
{
	// Setup rendering context
	RenderingContext rctx(
		m_swapchain,
		m_commandSystem,
		m_renderPasses,
		m_pipelines,
		m_uniformBuffers,
		m_descriptorSets,
		m_camera,
		m_framesInFlight);

	// Load scene
	APP_LOG_INFO("Loading scene");
	SimpleCubeScene scene; // Tavonput Scene
	// PyramidScene scene; // Alex Scene 
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

void Application::createPipelines()
{
	APP_LOG_INFO("Creating pipelines");

	// Create a pipeline builder
	auto builder = Pipeline::Builder(m_context.getDevice());

	builder.addGraphicsBase();
	builder.linkRenderPass(m_renderPasses[RenderPass::MAIN]);
	builder.linkDescriptorSetLayout(m_descriptorLayout);

	RasterShaderSet lightingShaders("../../Shaders/lighting_vert.spv", "../../Shaders/lighting_frag.spv", m_context.getDevice());
	builder.linkShaders(lightingShaders);

	builder.enableMultisampling(m_swapchain.getMSAASampleCount());

	// Build graphics pipeline - LIGHTING
	m_pipelines.push_back(builder.buildPipeline());

	// Link a different set of shaders
	RasterShaderSet flatShaders("../../Shaders/flat_vert.spv", "../../Shaders/flat_frag.spv", m_context.getDevice());
	builder.linkShaders(flatShaders);

	// Build graphics pipeline - FLAT
	m_pipelines.push_back(builder.buildPipeline());

	// Cleanup shader set
	lightingShaders.cleanup();
	flatShaders.cleanup();
}

void Application::createDescriptorSets()
{
	// Create descriptor set layout builder
	auto layoutBuilder = DescriptorSetLayout::Builder(m_context.getDevice());

	// Add a uniform buffer to the global binding
	layoutBuilder.addBinding(
		SceneBinding::GLOBAL,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

	// Build layout
	m_descriptorLayout = layoutBuilder.buildLayout();

	// Initialize descriptor pool
	m_descriptorPool.init(m_context.getDevice(), m_framesInFlight, 1);
	
	// Allocate and update descriptors set for each frame in flight
	for (uint8_t i = 0; i < m_framesInFlight; i++)
	{
		m_descriptorSets.push_back(m_descriptorPool.allocateDescriptorSet(m_descriptorLayout));

		m_descriptorSets[i].addBufferWrite(m_uniformBuffers[i], 0, SceneBinding::GLOBAL);
		m_descriptorSets[i].update(m_context.getDevice());
	}
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
	
	// Descriptor stuff
	m_descriptorLayout.cleanup(m_context.getDevice());
	m_descriptorPool.cleanup();

	// Uniform Buffers
	for (auto& buffer : m_uniformBuffers)
		buffer.cleanup();

	// Swapchain
	m_swapchain.cleanup();

	// System stuff
	m_context.cleanup();
	m_window.cleanup();

	APP_LOG_INFO("Application has been successfully cleaned up");
}
