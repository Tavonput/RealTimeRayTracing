#include "pch.h"

#include "Gui.h"

void Gui::init(Gui::CreateInfo info)
{
	APP_LOG_INFO("Initializing ImGui");

	m_device = &info.pSystemContext->getDevice();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows ***Causes memory crash. Needs further configuration
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForVulkan(info.pWindow->getWindowGLFW(), true);

	// Load and set font
	const char* fontPath = "../../../Assets/fonts/SourceSansPro-Black.otf";
	float fontSize = 20.0f;
	io.Fonts->AddFontFromFileTTF(fontPath, fontSize);

	// Setup descriptor pool
	DescriptorPool::CreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.pDevice                   = m_device;
	descriptorPoolInfo.flags                     = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolInfo.name                      = "GUI Descriptor Pool";
	descriptorPoolInfo.maxSets                   = 1;
	descriptorPoolInfo.poolSize                  = 1;
	descriptorPoolInfo.combinedImageSamplerCount = 1;
	m_descriptorPool.init(descriptorPoolInfo);
	
	// Setup ImGui Vulkan implementation
	ImGui_ImplVulkan_InitInfo initInfo{};
	initInfo.Instance       = info.pSystemContext->getInstance();
	initInfo.PhysicalDevice = m_device->getPhysical();
	initInfo.Device         = m_device->getLogical();
	initInfo.QueueFamily    = m_device->getIndices().graphicsFamily.value();
	initInfo.Queue          = m_device->getGraphicsQueue();
	initInfo.DescriptorPool = m_descriptorPool.getPool();
	initInfo.RenderPass     = info.pRenderPass->renderPass;
	initInfo.ImageCount     = info.imageCount;
	//initInfo.MinImageCount  = info.minImageCount;
	initInfo.MinImageCount = 2;
	initInfo.MSAASamples    = info.msaaSamples;
	ImGui_ImplVulkan_Init(&initInfo);

}



void Gui::beginUI()
{

	// Start a new frame
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_state.changed = false;

	// Determine the size of the main window 
	ImVec2 screenSize = ImGui::GetMainViewport()->Size;


	// Define the percentage of screen width and height for the settings window
	float widthPercentage = 0.2f;  // For example, 20% of the screen width
	float heightOffset = 26;  // Assuming 26 is the height of the main menu bar or top margin

	// Calculate the UI window size to be a percentage of the screen size
	ImVec2 window_size = ImVec2(screenSize.x * widthPercentage, screenSize.y - heightOffset);

	// Set position of the Ui to the top right corner
	// x is the screen width minus the window width to align to the right, Y is 0 to align to the top.
	ImVec2 window_pos = ImVec2(screenSize.x - window_size.x, heightOffset);

	// Set the UI window size.

	//ImVec2 window_size = ImVec2(300, 300);

	// Set position of the Ui to the top right corner
	// x is the screen width minus the window width to align to the right, Y is 0 to align to the top.
	//ImVec2 window_pos = ImVec2(screenSize.x - window_size.x, 0); 


	// Apply the calculated position and size to the next window (the "Settings" window).
	// ImGuiCond_Always means this size setting will be applied every time without any conditions.
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	// bool demo = true;

	// ImGui::ShowDemoWindow(&demo);

	{

		// Creating the settings window 
		// ImGuiWindowFlags_NoResize prevents the user from resizing the window. 

		//ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoResize);

		ImGui::Begin("Settings");

		// Debug settings
		if (ImGui::CollapsingHeader("Debug"))
		{
			const char* debugMethods[6] = { "None", "Albedo", "Normal", "Metallic", "Roughness", "Extra" };
			m_state.changed |= ImGui::Combo("Debug Modes (Raster & RTX Real Time)", (int*)&m_state.debugMode, debugMethods, 6);
		}

		// Scene settings
		if (ImGui::CollapsingHeader("Scene"))
		{
			m_state.changed |= ImGui::ColorEdit3("Background", m_state.backgroundColor);
			m_state.changed |= ImGui::SliderFloat("Exposure", &m_state.exposure, 0.1f, 5.0f);
		}

		// Lighting settings
		if (ImGui::CollapsingHeader("Lighting"))
		{
			m_state.changed |= ImGui::ColorEdit3("Light Color", m_state.lightColor);
			m_state.changed |= ImGui::DragFloat3("Light Position", m_state.lightPosition, 0.01f);
			m_state.changed |= ImGui::SliderFloat("Light Intensity", &m_state.lightIntensity, 0.0f, 20.0f);
		}

		// RTX Settings
		if (m_device->isRtxSupported())
			renderRtxUI();

		// Custom check boxes defined by the user
		if (ImGui::CollapsingHeader("Custom Check Boxes"))
		{
			for (CheckBox& box : m_customCheckBoxes)
				m_state.changed |= ImGui::Checkbox(box.name.c_str(), box.button);
		}

		// Camera Settings
		if (ImGui::CollapsingHeader("Camera"))
		{
			m_state.changed |= ImGui::SliderFloat("Camera Sensitivity", &m_state.sensitivity, 0.0f, 2.0f);
			m_state.changed |= ImGui::SliderFloat("Camera Speed", &m_state.speed, 0.0f, 6.0f);
			m_state.changed |= ImGui::InputFloat("Ground Height", &m_state.ground, 0.01f, 1.0f, "%.3f");
		 
			ImGui::SeparatorText("Camera Modes");
			m_state.changed |= ImGui::RadioButton("Stationary", &m_state.mode, 0); 
			m_state.changed |= ImGui::RadioButton("First Person View (FPV)", &m_state.mode, 1);

			ImGui::SeparatorText("Camera Positions");
			if (ImGui::Button("Save Position")) { m_state.cameraSaves++; }

			ImGui::Text("Switch Position: ");
			ImGui::SameLine();
			if (m_state.changed = ImGui::ArrowButton("##left", ImGuiDir_Left)) { m_state.currentCamera--; } //** If statement valid?
			ImGui::SameLine();
			if (m_state.changed = ImGui::ArrowButton("##right", ImGuiDir_Right)) { m_state.currentCamera++; }
		}


		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("RenderTime")) {
				// Display render time and FPS
				float framerate = ImGui::GetIO().Framerate;
				float renderTime = 1000.0f / framerate;
				ImGui::Text("Render Time: %.3f ms/frame", renderTime);
				ImGui::Text("FPS: %.1f", framerate);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("UI Custom")) {
				ImGuiStyle& style = ImGui::GetStyle();

				ImGui::ColorEdit4("Menu Bar Background", (float*)&style.Colors[ImGuiCol_MenuBarBg]);	
				ImGui::ColorEdit4("Header Background", (float*)&style.Colors[ImGuiCol_Header]);				
				ImGui::ColorEdit4("Header Hovered", (float*)&style.Colors[ImGuiCol_HeaderHovered]);				
				ImGui::ColorEdit4("Header Active", (float*)&style.Colors[ImGuiCol_HeaderActive]);				
				ImGui::ColorEdit4("Text Color", (float*)&style.Colors[ImGuiCol_Text]);				
				ImGui::ColorEdit4("Window Background", (float*)&style.Colors[ImGuiCol_WindowBg]);			
				ImGui::ColorEdit4("Title Background Active", (float*)&style.Colors[ImGuiCol_TitleBgActive]);				
				ImGui::ColorEdit4("Button Background", (float*)&style.Colors[ImGuiCol_Button]);
				ImGui::ColorEdit4("Button Hovered", (float*)&style.Colors[ImGuiCol_ButtonHovered]);
				ImGui::ColorEdit4("Button Active", (float*)&style.Colors[ImGuiCol_ButtonActive]);
				ImGui::ColorEdit4("Frame Background", (float*)&style.Colors[ImGuiCol_FrameBg]);
				ImGui::ColorEdit4("Frame Background Hovered", (float*)&style.Colors[ImGuiCol_FrameBgHovered]);
				ImGui::ColorEdit4("Frame Background Active", (float*)&style.Colors[ImGuiCol_FrameBgActive]);
				ImGui::ColorEdit4("Slider Grab", (float*)&style.Colors[ImGuiCol_SliderGrab]);
				ImGui::ColorEdit4("Slider Grab Active", (float*)&style.Colors[ImGuiCol_SliderGrabActive]);
				ImGui::ColorEdit4("Check Mark", (float*)&style.Colors[ImGuiCol_CheckMark]);
				ImGui::ColorEdit4("Scrollbar Background", (float*)&style.Colors[ImGuiCol_ScrollbarBg]);
				ImGui::ColorEdit4("Scrollbar Grab", (float*)&style.Colors[ImGuiCol_ScrollbarGrab]);
				ImGui::ColorEdit4("Scrollbar Grab Hovered", (float*)&style.Colors[ImGuiCol_ScrollbarGrabHovered]);
				ImGui::ColorEdit4("Scrollbar Grab Active", (float*)&style.Colors[ImGuiCol_ScrollbarGrabActive]);

				ImGui::EndMenu();
			}


			ImGui::EndMainMenuBar();
		}

		ImGui::End();
	}
}

void Gui::renderUI(VkCommandBuffer commandBuffer)
{
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer); //Third argument can be NULL.
}

void Gui::changeRenderMethod()
{
}

void Gui::setInitialLightPosition(glm::vec3 pos)
{
	m_state.lightPosition[0] = pos.x;
	m_state.lightPosition[1] = pos.y;
	m_state.lightPosition[2] = pos.z;
}

void Gui::setInitialBackground(glm::vec3 color)
{
	m_state.backgroundColor[0] = color.x;
	m_state.backgroundColor[1] = color.y;
	m_state.backgroundColor[2] = color.z;
}

void Gui::cleanup() 
{
	APP_LOG_INFO("Destroying ImGui");

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	m_descriptorPool.cleanup();
}

void Gui::renderRtxUI()
{
	if (ImGui::CollapsingHeader("RTX"))
	{
		const char* methods[3] = { "Raster", "RTX Real Time", "RTX Path" };
		m_state.changed |= ImGui::Combo("Render Method", (int*)&m_state.renderMethod, methods, 3);

		m_state.changed |= ImGui::SliderInt("Max Depth", &m_state.maxDepth, 1, 100);
		m_state.changed |= ImGui::SliderInt("Samples Per Pixel", &m_state.sampleCount, 1, 16);

		if (ImGui::TreeNode("Real Time"))
		{
			m_state.changed |= ImGui::SliderInt("TAA Frame Count", &m_state.TAAFrameCount, 1, 100);
			ImGui::SetItemTooltip("Number of accumulation frames for TAA");

			ImGui::TreePop();
			ImGui::Spacing();
		}

		if (ImGui::TreeNode("Path Tracer"))
		{
			m_state.changed |= ImGui::SliderFloat("Russian Roulette", &m_state.russianRoulette, 0.0f, 1.0f);
			ImGui::SetItemTooltip("Minimum Russian Roulette survival rate");

			m_state.changed |= ImGui::SliderInt("Max Path Frame Count", &m_state.maxPathFrame, 0, 100);
			ImGui::SetItemTooltip("Number of accumulation frames to compute for path tracing. Set to 0 for infinite");

			ImGui::TreePop();
			ImGui::Spacing();
		}
	}
}
