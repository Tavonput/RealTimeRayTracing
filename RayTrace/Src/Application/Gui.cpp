#include "pch.h"

#include "Gui.h"




void Gui::init(ImGui_ImplVulkan_InitInfo init_info, Window& m_window) {

	APP_LOG_INFO("Initializing ImGui. IT WORKS!!!!");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows ***Causes memory crash. Needs further configuration
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForVulkan(m_window.getWindowGLFW(), true);
	ImGui_ImplVulkan_Init(&init_info);

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

void Gui::renderUI(VkCommandBuffer m_commandBuffer)
{
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_commandBuffer); //Third argument can be NULL.

	return;
}



void Gui::cleanup() {
	/*ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();*/
}

