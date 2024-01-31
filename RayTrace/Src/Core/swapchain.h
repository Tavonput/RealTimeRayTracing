#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "device.h"
#include "image.h"

#include "Application/window.h"
#include "Application/logging.h"


struct SwapchainCreateInfo
{
	const Device*       device;
	const VkSurfaceKHR* surface;
	Window*             window;

	uint32_t framesInFlight = 1;

	Logger logger;
};

class Swapchain
{
public:
	void init(SwapchainCreateInfo& createInfo);

	void setupFramebuffers(const VkRenderPass& renderPass);
	void recreateSwapchain();

	uint32_t acquireImage(uint32_t& frameIndex);
	void submitGraphics(VkCommandBuffer commandBuffer, uint32_t& frameIndex);
	void present(uint32_t frameIndex, uint32_t& imageIndex);

	VkFormat getFormat();
	VkExtent2D getExtent();
	VkFramebuffer getFramebuffer(uint32_t index);

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

	bool m_recreate = false;

	Window*             m_window     = nullptr;
	const Device*       m_device     = nullptr;
	const VkSurfaceKHR* m_surface    = nullptr;
	const VkRenderPass* m_renderPass = nullptr;

	Logger m_logger;

	void setupSwapchain();
	void setupImageViews();
	void setupSyncObjects(uint32_t framesInFlight);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const Window& window);
};