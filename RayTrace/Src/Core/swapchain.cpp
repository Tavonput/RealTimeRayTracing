#include "swapchain.h"

void Swapchain::init(SwapchainCreateInfo& createInfo)
{
	m_device = createInfo.device;
	m_logger = createInfo.logger;

	setupSwapchain(*createInfo.window, *createInfo.surface);
	setupImageViews();
	setupSyncObjects(createInfo.framesInFlight);
}

void Swapchain::setupFramebuffers(const VkRenderPass& renderPass)
{
	m_framebuffers.resize(m_imageViews.size());

	// Create a framebuffer for each image view
	for (size_t i = 0; i < m_imageViews.size(); i++)
	{
		// Attachments
		std::array<VkImageView, 1> attachments = {
			m_imageViews[i],
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass      = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments    = attachments.data();
		framebufferInfo.width           = m_extent.width;
		framebufferInfo.height          = m_extent.height;
		framebufferInfo.layers          = 1;

		if (vkCreateFramebuffer(m_device->getLogical(), &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS)
		{
			LOG_CRITICAL("Failed to create framebuffers");
			throw;
		}
	}
}

void Swapchain::recreateSwapchain(const Window& window, const VkSurfaceKHR& surface, const VkRenderPass& renderPass)
{
	// Find new size of window
	int width = 0, height = 0;
	window.getSize(&width, &height);
	while (width == 0 || height == 0)
	{
		window.getSize(&width, &height);;
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_device->getLogical());

	// Cleanup old swapchain
	cleanup();

	// Create new swapchain
	setupSwapchain(window, surface);
	setupImageViews();
	setupFramebuffers(renderPass);
}

VkFormat Swapchain::getFormat()
{
	return m_format;
}

VkExtent2D Swapchain::getExtent()
{
	return m_extent;
}

void Swapchain::cleanup()
{
	LOG_INFO("Destroying swapchain");

	// Syncronization objects
	for (size_t i = 0; i < m_imageAvailableSemaphores.size(); i++)
	{
		vkDestroySemaphore(m_device->getLogical(), m_imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(m_device->getLogical(), m_renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(m_device->getLogical(), m_inFlightFences[i], nullptr);
	}

	// Framebuffers
	for (auto framebuffer : m_framebuffers)
		vkDestroyFramebuffer(m_device->getLogical(), framebuffer, nullptr);

	// Image Views
	for (auto imageView : m_imageViews)
		vkDestroyImageView(m_device->getLogical(), imageView, nullptr);

	// Swapchain
	vkDestroySwapchainKHR(m_device->getLogical(), m_swapchain, nullptr);
}

void Swapchain::setupSwapchain(const Window& window, const VkSurfaceKHR& surface)
{
	LOG_INFO("Initializing swapchain");

	// Choose a format, present mode, and extent
	SwapChainSupportDetails swapChainSupport = Device::querySwapChainSupport(m_device->getPhysical(), surface);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D scExtent = chooseSwapExtent(swapChainSupport.capabilities, window);

	// Get the image count
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;

	// Swap chain creation details
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface          = surface;
	createInfo.minImageCount    = imageCount;
	createInfo.imageFormat      = surfaceFormat.format;
	createInfo.imageColorSpace  = surfaceFormat.colorSpace;
	createInfo.imageExtent      = scExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = m_device->getIndicies();
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices   = queueFamilyIndices;
	}
	else
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	createInfo.preTransform   = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode    = presentMode;
	createInfo.clipped        = VK_TRUE;
	createInfo.oldSwapchain   = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_device->getLogical(), &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
	{
		LOG_CRITICAL("Failed to create swapchain");
		throw;
	}

	// Save the images, format, and extent
	vkGetSwapchainImagesKHR(m_device->getLogical(), m_swapchain, &imageCount, nullptr);
	m_images.resize(imageCount);
	vkGetSwapchainImagesKHR(m_device->getLogical(), m_swapchain, &imageCount, m_images.data());

	m_format = surfaceFormat.format;
	m_extent = scExtent;

	LOG_INFO("Swapchain initialization successful");
}

void Swapchain::setupImageViews()
{
	m_imageViews.resize(m_images.size());

	// Create images views for each image
	for (size_t i = 0; i < m_images.size(); i++)
	{
		Image::setupImageView(
			m_imageViews[i], m_images[i],
			m_format,
			VK_IMAGE_ASPECT_COLOR_BIT,
			1, 1,
			m_device->getLogical());
	}
}

void Swapchain::setupSyncObjects(uint32_t framesInFlight)
{
	m_imageAvailableSemaphores.resize(framesInFlight);
	m_renderFinishedSemaphores.resize(framesInFlight);
	m_inFlightFences.resize(framesInFlight);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	// For each frame in flight, create two semaphores and one fence
	for (size_t i = 0; i < framesInFlight; i++)
	{
		if (vkCreateSemaphore(m_device->getLogical(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_device->getLogical(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_device->getLogical(), &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
		{
			LOG_CRITICAL("Failed to create syncronization objects for frame {}", i);
			throw;
		}
	}
}

VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	// Find a format for RGBA8 and SRGB Nonlinear
	for (const auto& availableFormat : availableFormats)
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;

	// Return the first format as a backup
	return availableFormats[0];
}

VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	// Find mailbox present mode
	for (const auto& availablePresentMode : availablePresentModes)
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;

	// Return FIFO as a backup
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const Window& window)
{
	// Check if the extent is already set
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;
	else
	{
		int width, height;
		window.getSize(&width, &height);

		// Set the extent to the size of the framebuffer
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		// Ensure that the extent is within the min and max capabilities
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}