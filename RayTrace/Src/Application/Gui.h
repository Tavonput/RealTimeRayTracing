#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include "logging.h"

class Gui {

public: 
	void init(ImGui_ImplVulkan_InitInfo init_info);
	void cleanup();
private:

};