#pragma once

#include "logging.h"
#include "camera.h"
#include "event.h"
#include "model.h"

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
#include "Core/renderer.h"
#include "Core/descriptor.h"
#include "Core/rendering_structures.h"

//#include "ImGui/imconfig.h"
//#include "ImGui/imgui_tables.cpp"
//#include "ImGui/imgui_internal.h"
//#include "ImGui/imgui.cpp"
//#include "ImGui/imgui_draw.cpp"
//#include "ImGui/imgui_widgets.cpp"
//#include "ImGui/imgui_demo.cpp"
//#include "ImGui/imgui.h"
//#include "ImGui/imgui_impl_glfw.h"
//#include "ImGui/imgui_impl_vulkan.h"



class Application
{
public:
	// Create info
	struct CreateInfo
	{
		uint32_t windowHeight = 800;
		uint32_t windowWidth  = 600;

		uint32_t framesInFlight = 2;

		bool vSync = true;
	};

	void init(Application::CreateInfo& createInfo);

	//void initImgui(); //***Changes

	void run();

private:
	// System fields
	Window        m_window;
	SystemContext m_context;

	// Rendering fields
	Swapchain     m_swapchain;
	CommandSystem m_commandSystem;

	std::vector<Pipeline>   m_pipelines;
	std::vector<RenderPass> m_renderPasses;

	DescriptorSetLayout        m_descriptorLayout;
	DescriptorPool             m_descriptorPool;
	std::vector<DescriptorSet> m_descriptorSets;
	std::vector<Buffer>        m_uniformBuffers;
	Buffer                     m_materialDescriptionBuffer;

	Camera m_camera;

	uint32_t m_framesInFlight = 2;

	CornellBoxScene m_scene;
	// SimpleCubeScene m_scene;

	void createRenderPass();
	void createPipelines();
	void createDescriptorSets();
	void loadScene();

	void pollEvents();

	void cleanup();
};
