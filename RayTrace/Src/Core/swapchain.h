#pragma once

#include "device.h"
#include "image.h"
#include "depth_buffer.h"

#include "Application/window.h"
#include "Application/logging.h"

class Swapchain
{
public:
	// Create info
	struct CreateInfo
	{
		const Device*       device;
		const VkSurfaceKHR* surface;
		Window*             window;

		uint32_t framesInFlight = 1;

		bool vSync = true;
	};

	void init(Swapchain::CreateInfo& createInfo);

	void setupFramebuffers(const VkRenderPass& renderPass);
	void recreateSwapchain();

	uint32_t acquireImage(uint32_t& frameIndex);
	void submitGraphics(VkCommandBuffer commandBuffer, uint32_t& frameIndex);
	void present(uint32_t frameIndex, uint32_t& imageIndex);

	VkFormat getFormat();
	VkFormat getDepthFormat();
	VkExtent2D getExtent();
	VkFramebuffer getFramebuffer(uint32_t index);
	VkSampleCountFlagBits getMSAASampleCount();


	void cleanup();

private:
	// Swapchain
	VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;

	std::vector<Image>         m_images;
	std::vector<VkFramebuffer> m_framebuffers;

	VkFormat   m_format = VK_FORMAT_UNDEFINED;
	VkExtent2D m_extent = { 0, 0 };

	// Depth buffer
	DepthBuffer m_depthBuffer;

	// Multisampling
	Image                 m_MSAAImage;
	VkSampleCountFlagBits m_MSAASampleCount = VK_SAMPLE_COUNT_1_BIT;

	// Syncronization
	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence>     m_inFlightFences;

	// Recreation
	bool m_recreate = false;

	// Context
	Window*             m_window     = nullptr;
	const Device*       m_device     = nullptr;
	const VkSurfaceKHR* m_surface    = nullptr;
	const VkRenderPass* m_renderPass = nullptr;

	bool m_vSync = true;

	void setupSwapchain();
	void setupImageViews();
	void setupSyncObjects(uint32_t framesInFlight);
	void setupMSAA();

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const Window& window);
};