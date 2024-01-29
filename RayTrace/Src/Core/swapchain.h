#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "device.h"
#include "image.h"

#include "Application/window.h"
#include "Application/logging.h"

struct SwapchainCreateInfo
{
	const Device* device;
	const Window* window;
	const VkSurfaceKHR* surface;

	uint32_t framesInFlight = 1;

	Logger logger;
};

class Swapchain
{
public:
	void init(SwapchainCreateInfo& createInfo);

	void setupFramebuffers(const VkRenderPass& renderPass);
	void recreateSwapchain(const Window& window, const VkSurfaceKHR& surface, const VkRenderPass& renderPass);

	VkFormat getFormat();
	VkExtent2D getExtent();

	void cleanup();

private:
	VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;

	std::vector<VkImage>       m_images;
	std::vector<VkImageView>   m_imageViews;
	std::vector<VkFramebuffer> m_framebuffers;

	VkFormat   m_format = VK_FORMAT_UNDEFINED;
	VkExtent2D m_extent = { 0, 0 };

	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence>     m_inFlightFences;

	const Device* m_device = nullptr;

	Logger m_logger;

	void setupSwapchain(const Window& window, const VkSurfaceKHR& surface);
	void setupImageViews();
	void setupSyncObjects(uint32_t framesInFlight);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const Window& window);
};