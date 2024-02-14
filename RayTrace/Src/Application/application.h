#pragma once

#include "logging.h"
#include "camera.h"

#include "Scene/simple_cube_scene.h"
#include "Scene/pyramid_scene.h"

#include "Core/system_context.h"
#include "Core/swapchain.h"
#include "Core/render_pass.h"
#include "Core/command.h"
#include "Core/pipeline.h"
#include "Core/shader.h"
#include "Core/buffer.h"
#include "Core/renderer.h"
#include "Core/descriptor.h"
#include "Core/rendering_structures.h"

struct ApplicationCreateInfo
{
	uint32_t windowHeight = 800;
	uint32_t windowWidth  = 600;

	uint32_t framesInFlight = 2;
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

	DescriptorSetLayout        m_descriptorLayout;
	DescriptorPool             m_descriptorPool;
	std::vector<DescriptorSet> m_descriptorSets;
	std::vector<Buffer>        m_uniformBuffers;

	Camera m_camera;

	uint32_t m_framesInFlight = 2;

	void createRenderPass();
	void createPipelines();
	void createDescriptorSets();

	void cleanup();
};
