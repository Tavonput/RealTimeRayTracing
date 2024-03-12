#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include "logging.h"
#include "window.h"

#include "Core/system_context.h"
#include "Core/render_pass.h"
#include "Core/descriptor.h"

/*
* General flow to follow 

At initialization :
call ImGui::CreateContext()
call ImGui_ImplXXXX_Init() for each backend.

At the beginning of your frame :
call ImGui_ImplXXXX_NewFrame() for each backend.
call ImGui::NewFrame()

At the end of your frame :
call ImGui::Render()
call ImGui_ImplXXXX_RenderDrawData() for your Renderer backend.

At shutdown :
call ImGui_ImplXXXX_Shutdown() for each backend.
call ImGui::DestroyContext()
*/

class Gui {

public: 
	struct CreateInfo
	{
		SystemContext*        pSystemContext;
		Window*               pWindow;
		RenderPass*           pRenderPass;
		uint32_t              minImageCount;
		uint32_t              imageCount;                   
		VkSampleCountFlagBits msaaSamples;
	};

	void init(Gui::CreateInfo info);

	void cleanup();

	void beginUI();
	void renderUI(VkCommandBuffer commandBuffer);
	void changeRenderMethod();

private:
	const Device* m_device = nullptr;

	DescriptorPool m_descriptorPool;
};