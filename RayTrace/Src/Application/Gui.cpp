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
	initInfo.MinImageCount  = info.minImageCount;
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

	// bool showDemoWindow = true;
	// ImGui::ShowDemoWindow(&showDemoWindow);

	{
		ImGui::Begin("Settings");

		// Scene settings
		if (ImGui::CollapsingHeader("Scene"))
		{
			m_state.changed |= ImGui::ColorEdit3("Background", m_state.backgroundColor);
		}

		// Lighting settings
		if (ImGui::CollapsingHeader("Lighting"))
		{
			m_state.changed |= ImGui::ColorEdit3("Light Color", m_state.lightColor);
			m_state.changed |= ImGui::DragFloat3("Light Position", m_state.lightPosition, 0.01f);
			m_state.changed |= ImGui::SliderFloat("Light Intensity", &m_state.lightIntensity, 0.0f, 5.0f);
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
		m_state.changed |= ImGui::Checkbox("Enable RTX", &m_state.useRtx);
		m_state.changed |= ImGui::SliderInt("Max Depth", &m_state.maxDepth, 1, 50);
		m_state.changed |= ImGui::SliderInt("Samples Per Pixel", &m_state.sampleCount, 1, 16);
		m_state.changed |= ImGui::SliderInt("TAA Frame Count", &m_state.TAAFrameCount, 1, 100);
	}
}
