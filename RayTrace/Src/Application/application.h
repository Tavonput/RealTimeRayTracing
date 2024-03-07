#pragma once

#include "logging.h"
#include "camera.h"
#include "event.h"
#include "model.h"
#include "Gui.h"

#include "Cpu-Raytracing/cpu_raytracer.h"

#include "Scene/simple_cube_scene.h"
#include "Scene/cornell_box.h"
// #include "Scene/pyramid_scene.h"

#include "Core/system_context.h"
#include "Core/swapchain.h"
#include "Core/render_pass.h"
#include "Core/command.h"
#include "Core/pipeline.h"
#include "Core/shader.h"
#include "Core/buffer.h"
#include "Core/image.h"
#include "Core/depth_buffer.h"
#include "Core/renderer.h"
#include "Core/descriptor.h"
#include "Core/rendering_structures.h"
#include "Core/framebuffer.h"
#include "Core/texture.h"

class Application
{
public:
	// Settings
	struct Settings
	{
		uint32_t windowHeight = 800;
		uint32_t windowWidth  = 600;

		uint32_t framesInFlight = 2;

		bool vSync         = true;
		bool cpuRaytracing = false;
	};

	void init(Application::Settings& settings);

	void run();

private:
	// System fields
	Settings      m_settings;
	Window        m_window;
	SystemContext m_context;
	Gui           m_gui;

	// Rendering components
	Swapchain               m_swapchain;
	CommandSystem           m_commandSystem;
	DescriptorPool          m_descriptorPool;
	std::vector<Pipeline>   m_pipelines;
	std::vector<RenderPass> m_renderPasses;
	Camera                  m_camera;

	// Post pass
	std::vector<Framebuffer>   m_postFramebuffers;
	DescriptorSetLayout        m_postDescriptorLayout;
	std::vector<DescriptorSet> m_postDescriptorSets;

	// Main offscreen pass
	Framebuffer                m_offscreenFramebuffer;
	Texture                    m_offscreenColorTexture;
	DepthBuffer                m_offscreenDepthBuffer;
	DescriptorSetLayout        m_offscreenDescriptorLayout;
	std::vector<DescriptorSet> m_offscreenDescriptorSets;
	std::vector<Buffer>        m_uniformBuffers;
	Buffer                     m_materialDescriptionBuffer;

  // Scenes
	CornellBoxScene m_scene;
	// SimpleCubeScene m_scene;

	CpuRaytracer m_cpuRaytracer;

	void createRenderPasses();
	void createPipelines();
	void createDescriptorSets();
	void createFramebuffers();

	void setupOffscreenRender();
	void resetOffscreenRender();

	void loadScene();

	void pollEvents();

	void cleanup();
};
