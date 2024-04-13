#pragma once

#include <glm/glm.hpp>

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
	enum class RenderMethod
	{
		RASTER = 0,
		RTX_RT,
		RTX_PATH
	};

	enum class DebugMode
	{
		NONE = 0,
		ALBEDO,
		NORMAL,
		METAL,
		ROUGH,
		EXTRA
	};

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

		// Debug
		DebugMode debugMode = DebugMode::NONE;

		// Scene
		float backgroundColor[3] = { 1.0f, 1.0f, 1.0f };
		float exposure           = 1.0;

		// Lighting
		float lightColor[3]    = { 1.0f, 1.0f, 1.0f };
		float lightPosition[3] = { 0.0f, 0.0f, 0.0f };
		float lightIntensity   = 1.0f;

		// RTX
		RenderMethod renderMethod    = RenderMethod::RASTER;
		int          maxDepth        = 10;
		int          sampleCount     = 4;
		int          TAAFrameCount   = 10;
		int          maxPathFrame    = 0;
		float        russianRoulette = 0.3f;

		// Camera
		float sensitivity   = 1.0f;
		float speed         = 3.0f;
		float jumpSpeed     = 0.5f;
		float gravity       = 1.0f;
		float ground        = 0.0f;
		int   mode          = 0;
		int   cameraSaves   = 0;
		int   currentCamera = 0;

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

	/**
	 * Allow the user to specify custom check boxes.
	 * 
	 * @param name: Name for the check box.
	 * @param button: The boolean attached to the button.
	 */
	void addCustomCheckBox(const std::string& name, bool* button) { m_customCheckBoxes.emplace_back(name, button); }

	void changeRenderMethod();

	void setInitialLightPosition(glm::vec3 pos);
	void setInitialBackground(glm::vec3 color);

	void cleanup();

private:
	// Used for storing custom check boxes from the user
	struct CheckBox
	{
		std::string name;
		bool*       button;

		CheckBox(const std::string& _name, bool* _button)
			: name(_name), button(_button) {}
	};

	const Device* m_device = nullptr;

	DescriptorPool m_descriptorPool;

	UiState               m_state;
	std::vector<CheckBox> m_customCheckBoxes;

	void renderRtxUI();
};
