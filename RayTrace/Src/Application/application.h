#pragma once

#include "logging.h"
#include "context.h"

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
	uint32_t windowWidth  = 800;

	uint32_t framesInFlight = 1;
};

class Application
{
public:
	void init(ApplicationCreateInfo& createInfo);

	void run();

private:
	Logger  m_logger;

	Context m_context;

	// Rendering
	Swapchain           m_swapchain;
	RenderPass::Manager m_renderPassManager;
	CommandManager      m_commandManager;
	Pipeline            m_pipeline;

	Buffer m_vertexBuffer;

	uint32_t m_currentFrame   = 0;
	uint32_t m_framesInFlight = 1;

	void createRenderPass();
	void createVertexBuffer();

	void cleanup();
};

