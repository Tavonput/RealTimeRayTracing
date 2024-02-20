#include "pch.h"

#include "swapchain.h"

void Swapchain::init(Swapchain::CreateInfo& createInfo)
{
	m_device  = createInfo.device;
	m_window  = createInfo.window;
	m_surface = createInfo.surface;
	m_vSync   = createInfo.vSync;

	setupSwapchain();
	setupImageViews();
	setupSyncObjects(createInfo.framesInFlight);

	setupMSAA();
	m_depthBuffer = DepthBuffer(*m_device, m_extent, m_MSAASampleCount);
}

void Swapchain::setupFramebuffers(const VkRenderPass& renderPass)
{
	m_renderPass = &renderPass;
	m_framebuffers.resize(m_images.size());

	// Create a framebuffer for each image view
	for (size_t i = 0; i < m_images.size(); i++)
	{
		// Attachments
		std::array<VkImageView, 3> attachments = {
			m_MSAAImage.view,
			m_depthBuffer.image.view,
			m_images[i].view,
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
			APP_LOG_CRITICAL("Failed to create framebuffers");
			throw;
		}
	}
}

void Swapchain::onWindowResize(WindowResizeEvent event)
{
	recreateSwapchain();
}

void Swapchain::recreateSwapchain()
{
	m_recreate = true;

	// Find new size of window
	int width = 0, height = 0;
	m_window->getSize(&width, &height);
	while (width == 0 || height == 0)
	{
		m_window->getSize(&width, &height);;
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_device->getLogical());

	// Cleanup old swapchain
	cleanup();
	m_recreate = false;

	// Create new swapchain
	setupSwapchain();
	setupImageViews();

	setupMSAA();
	m_depthBuffer = DepthBuffer(*m_device, m_extent, m_MSAASampleCount);

	setupFramebuffers(*m_renderPass);
}

uint32_t Swapchain::acquireImage(uint32_t& frameIndex)
{
	// Try to get an image. Return when an image is acquired
	while (true)
	{
		// Wait for an available image
		vkWaitForFences(m_device->getLogical(), 1, &m_inFlightFences[frameIndex], VK_TRUE, UINT64_MAX);

		// Get an image
		VkResult result;
		uint32_t imageIndex;
		result = vkAcquireNextImageKHR(m_device->getLogical(), m_swapchain, UINT64_MAX, m_imageAvailableSemaphores[frameIndex], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
			continue;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			APP_LOG_ERROR("Failed to acquire swap chain image");
		}

		// Un-signal fence
		vkResetFences(m_device->getLogical(), 1, &m_inFlightFences[frameIndex]);

		return imageIndex;
	}

}

void Swapchain::submitGraphics(VkCommandBuffer commandBuffer, uint32_t& frameIndex)
{
	// End command buffer
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to record command buffer");

	// Submit command to graphics queue
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[]      = { m_imageAvailableSemaphores[frameIndex] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores    = waitSemaphores;
	submitInfo.pWaitDstStageMask  = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer;

	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[frameIndex] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores    = signalSemaphores;

	if (vkQueueSubmit(m_device->getGraphicsQueue(), 1, &submitInfo, m_inFlightFences[frameIndex]) != VK_SUCCESS)
		APP_LOG_CRITICAL("Failed to submit to graphics queue");
}

void Swapchain::present(uint32_t frameIndex, uint32_t& imageIndex)
{
	// Present frame
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = &m_renderFinishedSemaphores[frameIndex];

	VkSwapchainKHR swapChains[] = { m_swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains    = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	VkResult result;
	result = vkQueuePresentKHR(m_device->getPresentQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		recreateSwapchain();
	else if (result != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to present frame");
		throw;
	}
}

VkFormat Swapchain::getFormat()
{
	return m_format;
}

VkFormat Swapchain::getDepthFormat()
{
	return m_depthBuffer.format;
}

VkExtent2D Swapchain::getExtent()
{
	return m_extent;
}

VkFramebuffer Swapchain::getFramebuffer(uint32_t index)
{
	return m_framebuffers[index];
}

VkSampleCountFlagBits Swapchain::getMSAASampleCount()
{
	return m_MSAASampleCount;
}

void Swapchain::cleanup()
{
	APP_LOG_INFO("Destroying swapchain");

	// Framebuffers
	for (auto framebuffer : m_framebuffers)
		vkDestroyFramebuffer(m_device->getLogical(), framebuffer, nullptr);

	// Manually cleanup image views
	for (auto& image : m_images)
		vkDestroyImageView(m_device->getLogical(), image.view, nullptr);

	// Swapchain
	vkDestroySwapchainKHR(m_device->getLogical(), m_swapchain, nullptr);

	// Depth buffer
	m_depthBuffer.cleanup();

	// MSAA
	m_MSAAImage.cleanup(m_device->getLogical());

	// Don't destroy syncronization object during recreation
	if (m_recreate)
		return;

	// Syncronization objects
	for (size_t i = 0; i < m_imageAvailableSemaphores.size(); i++)
	{
		vkDestroySemaphore(m_device->getLogical(), m_imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(m_device->getLogical(), m_renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(m_device->getLogical(), m_inFlightFences[i], nullptr);
	}
}

void Swapchain::setupSwapchain()
{
	APP_LOG_INFO("Initializing swapchain");

	// Choose a format, present mode, and extent
	SwapChainSupportDetails swapChainSupport = Device::querySwapChainSupport(m_device->getPhysical(), *m_surface);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D scExtent = chooseSwapExtent(swapChainSupport.capabilities, *m_window);

	// Get the image count
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;

	// Swap chain creation details
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface          = *m_surface;
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
		APP_LOG_CRITICAL("Failed to create swapchain");
		throw;
	}

	// Save the images, format, and extent
	vkGetSwapchainImagesKHR(m_device->getLogical(), m_swapchain, &imageCount, nullptr);
	m_images.resize(imageCount);

	std::vector<VkImage> swapchainVkImages(imageCount);
	vkGetSwapchainImagesKHR(m_device->getLogical(), m_swapchain, &imageCount, swapchainVkImages.data());

	for (uint8_t i = 0; i < m_images.size(); i++)
		m_images[i].image = swapchainVkImages[i];

	m_format = surfaceFormat.format;
	m_extent = scExtent;

	APP_LOG_INFO("Swapchain initialization successful");
}

void Swapchain::setupImageViews()
{
	Image::ImageViewSetupInfo createInfo{};
	createInfo.format      = m_format;
	createInfo.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.layerCount  = 1;
	createInfo.mipLevels   = 1;
	createInfo.device      = m_device;

	// Create images views for each image
	for (size_t i = 0; i < m_images.size(); i++)
		Image::SetupImageView(m_images[i], createInfo);
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
			APP_LOG_CRITICAL("Failed to create syncronization objects for frame {}", i);
			throw;
		}
	}
}

void Swapchain::setupMSAA()
{
	// Find max sample count
	m_MSAASampleCount = m_device->getMaxUsableSampleCount();

	// Create image
	Image::CreateInfo imgCreateInfo{};
	imgCreateInfo.width      = m_extent.width;
	imgCreateInfo.height     = m_extent.height;
	imgCreateInfo.mipLevels  = 1;
	imgCreateInfo.layerCount = 1;
	imgCreateInfo.numSamples = m_MSAASampleCount;
	imgCreateInfo.format     = m_format;
	imgCreateInfo.tiling     = VK_IMAGE_TILING_OPTIMAL;
	imgCreateInfo.usage      = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	imgCreateInfo.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	imgCreateInfo.device     = m_device;

	m_MSAAImage = Image::CreateImage(imgCreateInfo);

	// Setup image view
	Image::ImageViewSetupInfo viewSetupInfo{};
	viewSetupInfo.format      = m_format;
	viewSetupInfo.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	viewSetupInfo.mipLevels   = 1;
	viewSetupInfo.layerCount  = 1;
	viewSetupInfo.device      = m_device;

	Image::SetupImageView(m_MSAAImage, viewSetupInfo);
}

VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	// Find a format for RGBA8 in linear space and SRGB non-linear for presentation
	for (const auto& availableFormat : availableFormats)
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;

	APP_LOG_TRACE("Image format not found. Use backup format");

	// Return the first format as a backup
	return availableFormats[0];
}

VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	if (m_vSync)
		return VK_PRESENT_MODE_FIFO_KHR;

	// Find mailbox present mode
	for (const auto& availablePresentMode : availablePresentModes)
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;

	APP_LOG_TRACE("Present mode not found. Using backup VK_PRESENT_MODE_FIFO_KHR");

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