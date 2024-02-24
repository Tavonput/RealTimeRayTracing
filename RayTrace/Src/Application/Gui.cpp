#include "pch.h"

#include "Gui.h"




void Gui::init(ImGui_ImplVulkan_InitInfo init_info) {
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO();
	//ImGui::StyleColorsDark();
	//ImGui::NewFrame();

	//ImGui::Render();


	//imgui_implglfw_initforvulkan(m_window.getwindowglfw(), true);

	//imgui_implvulkan_initinfo info = {};
	//info.descriptorpool = m_descriptorpool.getpool(); //vkdescriptorpool
	//info.renderpass = m_renderpasses[0].renderpass;   //vkrenderpass
	//info.device = m_context.getdevice().getlogical(); //vkdevice
	//info.physicaldevice = m_context.getdevice().getphysical(); //vkphysicaldevice
	//info.imagecount = m_framesinflight;
	//info.msaasamples = m_context.getdevice().getmaxusablesamplecount(); //vksamplecountflagbits

	//imgui_implvulkan_init(&info);

	//vkcommandbuffer commandbuffer = m_commandsystem.beginsingletimecommands();
	//imgui_implvulkan_createfontstexture();
	//m_commandsystem.endsingletimecommands(commandbuffer, m_context.getdevice().getgraphicsqueue());

	//vkdevicewaitidle(m_context.getdevice().getlogical());
	//imgui_implvulkan_destroyfontuploadobjects();
	//imgui_implvulkan_destroyfontstexture();

}

void Gui::cleanup() {
	/*ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();*/
}

