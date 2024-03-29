#pragma once

#include "logging.h"
#include "camera.h"
#include "event.h"
#include "model.h"
#include "Gui.h"

#include "Cpu-Raytracing/cpu_raytracer.h"

#include "Scene/simple_cube_scene.h"
#include "Scene/cornell_box.h"
#include "Scene/dragon.h"
#include "Scene/model_viewer.h"
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
#include "Core/acceleration_structure.h"

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
		bool useRtx        = false;
	};

	void init(Application::Settings& settings);

	void run();

private:
	// System fields
	Settings      m_settings;
	Window        m_window;
	SystemContext m_context;
	Gui           m_gui;
	SceneBuilder  m_sceneBuilder;
	const Device* m_device = nullptr;

	// Rendering components
	Swapchain               m_swapchain;
	CommandSystem           m_commandSystem;
	DescriptorPool          m_descriptorPool;
	std::vector<Pipeline>   m_pipelines;
	std::vector<RenderPass> m_renderPasses;
	Camera                  m_camera;
	Renderer                m_renderer;

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
	Buffer                     m_objectDescBuffer;

    // Scenes
	CornellBoxScene m_scene;
	// ModelViewerScene m_scene; // Check out models from https://casual-effects.com/data/index.html
	// DragonScene      m_scene; // Requires "Chinese Dragon" from https://casual-effects.com/data/index.html
	// SimpleCubeScene  m_scene; // Kind of broken right now.

	// Cpu Raytracing
	CpuRaytracer m_cpuRaytracer;

	// Gpu Raytracing
	AccelerationStructure m_accelerationStructure;
	ShaderBindingTable    m_realTimeSBT;
	ShaderBindingTable    m_pathSBT;
	DescriptorPool        m_rtxDescriptorPool;
	DescriptorSetLayout   m_rtxDescriptorLayout;
	DescriptorSet         m_rtxDescriptorSet;
	
	void createRenderPasses();
	void createPipelines();
	void createDescriptorSets();
	void createFramebuffers();

	void createRtxDescriptorSets();
	void createRtxPipeline();

	void setupOffscreenRender();
	void resetOffscreenRender();

	void pollEvents();

	void cleanup();
};
