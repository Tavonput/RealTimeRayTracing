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

/*****************************************************************************************************************
 *
 * @class Gui
 *
 * Contains all UI rendering logic. Implements ImGui.
 *
 * Fill out the create info, initialize the Gui, then use it within the main rendering loop. Call beginUI() at the
 * start of each frame, and renderUI() when you want the UI to be rendered. Retrieve the current state of the
 * UI with getUIState().
 * 
 * The creator of the Gui is responsible for calling its cleanup().
 *
 * Example Usage:
 *     Gui::CreateInfo info{};
 *     info.pSystemContext = ...;
 *     info.pWindow        = ...;
 *     info.pRenderPass    = ...;
 *     ...
 *     Gui myGui;
 *     myGui.init(info);
 *     
 *     ...
 *     
 *     UiState currentUIState = myGui.getUIState();
 *     myGui.beginUI();
 *     
 *     ...
 *     
 *     myGui.renderUI();
 *     
 *     ...
 *     
 *     myGui.cleanup();
 *
 */
class Gui 
{
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

	struct UiState
	{
		bool changed = false;

		// RTX
		bool useRtx        = false;
		int  maxDepth      = 2;
		int  sampleCount   = 1;
		int  TAAFrameCount = 10;
	};

	void init(Gui::CreateInfo info);

	/**
	 * Get the current state of the UI.
	 * 
	 * @return UiState struct.
	 */
	const UiState& getUIState() const { return m_state; }

	/**
	 * Start a new Gui frame.
	 */
	void beginUI();

	/**
	 * Render the UI.
	 * 
	 * @param commandBuffer: Command buffer to record the UI draw commands.
	 */
	void renderUI(VkCommandBuffer commandBuffer);

	void changeRenderMethod();

	void cleanup();

private:
	const Device* m_device = nullptr;

	DescriptorPool m_descriptorPool;

	UiState m_state;

	void renderRtxUI();
};