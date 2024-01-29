#include "application.h"

void Application::init(ApplicationCreateInfo& createInfo)
{
	// Logger
	m_logger.init(spdlog::level::trace);
	LOG_INFO("Logging initialization successful");

	// Context
	m_context.init(createInfo.windowHeight, createInfo.windowWidth, m_logger);

	// Swapchain
	SwapchainCreateInfo swapchainCreateInfo;
	swapchainCreateInfo.device         = &m_context.getDevice();
	swapchainCreateInfo.window         = &m_context.getWindow();
	swapchainCreateInfo.surface        = &m_context.getSurface();
	swapchainCreateInfo.framesInFlight = createInfo.framesInFlight;
	swapchainCreateInfo.logger         = m_logger;

	m_swapchain.init(swapchainCreateInfo);
}

void Application::run()
{
	cleanup();
}

void Application::cleanup()
{
	m_swapchain.cleanup();

	m_context.cleanup();

	LOG_INFO("Application has been successfully cleaned up");
}
