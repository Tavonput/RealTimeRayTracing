#pragma once

#include "logging.h"

#include "Scene/example_scene.h"

#include "Core/system_context.h"
#include "Core/swapchain.h"
#include "Core/render_pass.h"
#include "Core/command.h"
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
	Window        m_window;
	SystemContext m_context;

	// Rendering fields
	Swapchain           m_swapchain;
	CommandSystem       m_commandSystem;

	std::vector<Pipeline>   m_pipelines;
	std::vector<RenderPass> m_renderPasses;

	uint32_t m_framesInFlight = 1;

	void createRenderPass();
	void createPipeline();

	void cleanup();
};

