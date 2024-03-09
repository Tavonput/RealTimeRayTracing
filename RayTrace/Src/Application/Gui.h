#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include "logging.h"
#include "window.h"

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
	void init(ImGui_ImplVulkan_InitInfo init_info, Window& m_window);
	void cleanup();
	void beginUI();
	void renderUI(VkCommandBuffer);
	void changeRenderMethod();
private:

};