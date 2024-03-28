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

	bool showDemoWindow = true;
	ImGui::ShowDemoWindow(&showDemoWindow);
	//return;

	{
		ImGui::Begin("Settings");

		// Debug settings
		if (ImGui::CollapsingHeader("Debug"))
		{
			const char* debugMethods[3] = { "None", "Albedo", "Normal" };
			m_state.changed |= ImGui::Combo("Debug Maps", (int*)&m_state.debugMode, debugMethods, 3);
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
			ImGui::Text("Camera Modes:"); ImGui::SameLine();
			m_state.changed |= ImGui::RadioButton("Stationary", &m_state.mode, 0); ImGui::SameLine();
			m_state.changed |= ImGui::RadioButton("First Person View (FPV)", &m_state.mode, 1); 
			
			if (ImGui::Button("Save Camera Position")) { m_state.cameraSaves++; }

			ImGui::Text("Switch Camera: ");
			ImGui::SameLine();
			if (m_state.changed = ImGui::ArrowButton("##left", ImGuiDir_Left)) { m_state.currentCamera--; } //** If statement valid?
			ImGui::SameLine();
			if (m_state.changed = ImGui::ArrowButton("##right", ImGuiDir_Right)) { m_state.currentCamera++; }


		}
		// Framerate
		ImGui::Text("Render Time %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

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
