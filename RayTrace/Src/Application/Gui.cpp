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
	bool showDemoWindow = true;
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow(&showDemoWindow);
	return;
}

void Gui::renderUI(VkCommandBuffer commandBuffer)
{
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer); //Third argument can be NULL.

	return;
}

void Gui::changeRenderMethod()
{
}



void Gui::cleanup() 
{
	APP_LOG_INFO("Destroying ImGui");

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	m_descriptorPool.cleanup();
}
