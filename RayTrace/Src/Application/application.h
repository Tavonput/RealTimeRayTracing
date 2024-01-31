#pragma once

#include "logging.h"

#include "Core/system_context.h"
#include "Core/swapchain.h"
#include "Core/render_pass.h"
#include "Core/command_manager.h"
#include "Core/pipeline.h"
#include "Core/vertex.h"
#include "Core/buffer.h"
#include "Core/renderer.h"

struct ApplicationCreateInfo
{
	uint32_t windowHeight = 800;
	uint32_t windowWidth  = 600;

	uint32_t framesInFlight = 1;
};

class Application
{
public:
	void init(ApplicationCreateInfo& createInfo);

	void run();

private:
	// System fields
	Logger        m_logger;
	Window        m_window;
	SystemContext m_context;

	// Rendering fields
	Swapchain           m_swapchain;
	RenderPass::Manager m_renderPassManager;
	Pipeline::Manager   m_pipelineManager;
	CommandManager      m_commandManager;

	uint32_t m_framesInFlight = 1;

	// Scene data fields
	Buffer m_vertexBuffer;

	void createRenderPass();
	void createPipeline();
	void createVertexBuffer();

	void cleanup();
};

