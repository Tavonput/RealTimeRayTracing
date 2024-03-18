#pragma once

#include "device.h"
#include "image.h"
#include "depth_buffer.h"
#include "framebuffer.h"

#include "Application/window.h"
#include "Application/logging.h"
#include "Application/event.h"

class Swapchain
{
public:
	// Create info
	struct CreateInfo
	{
		const Device*       device;
		const VkSurfaceKHR* surface;
		Window*             window;

		uint32_t framesInFlight = 2;

		bool vSync = true;
		bool msaa  = false;
	};

	void init(Swapchain::CreateInfo& createInfo);

	void onWindowResize(WindowResizeEvent event);

	uint32_t acquireImage(uint32_t& frameIndex);
	void submitGraphics(VkCommandBuffer commandBuffer, uint32_t& frameIndex);
	void present(uint32_t frameIndex, uint32_t& imageIndex);

	// Getters
	VkFormat getFormat();
	VkFormat getDepthFormat();
	VkExtent2D getExtent();
	uint32_t getMinImageCount() const { return m_minImage; }
	VkSampleCountFlagBits getMSAASampleCount();
	const uint32_t getImageCount() const { return static_cast<uint32_t>(m_images.size()); }
	const Image& getImage(uint32_t index) { return m_images[index]; }
	const Image& getMSAAImage() const { return m_MSAAImage; }
	const DepthBuffer& getDepthBuffer() const { return m_depthBuffer; }

	void cleanup();

private:
	// Swapchain
	VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;

	std::vector<Image>       m_images;

	VkFormat   m_format = VK_FORMAT_UNDEFINED;
	VkExtent2D m_extent = { 0, 0 };
	uint32_t   m_minImage = 0;

	// Depth buffer
	DepthBuffer m_depthBuffer;

	// Multisampling
	Image                 m_MSAAImage;
	VkSampleCountFlagBits m_MSAASampleCount = VK_SAMPLE_COUNT_1_BIT;
	bool                  m_msaaEnabled     = false;

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
	const RenderPass*   m_renderPass = nullptr;

	bool m_vSync = true;

	void setupSwapchain();
	void setupImageViews();
	void setupSyncObjects(uint32_t framesInFlight);
	void setupMSAA();

	void recreateSwapchain();

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const Window& window);
};