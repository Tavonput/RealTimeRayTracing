#include "pch.h"

#include "swapchain.h"

void Swapchain::init(SwapchainCreateInfo& createInfo)
{
	m_device  = createInfo.device;
	m_window  = createInfo.window;
	m_surface = createInfo.surface;
	m_logger  = createInfo.logger;

	setupSwapchain();
	setupImageViews();
	setupSyncObjects(createInfo.framesInFlight);
}

void Swapchain::setupFramebuffers(const VkRenderPass& renderPass)
{
	m_renderPass = &renderPass;
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
			LOG_ERROR("Failed to acquire swap chain image");
		}

		// Unsignal fence
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
		LOG_CRITICAL("Failed to submit to graphics queue");
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

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window->framebufferResized)
	{
		m_window->framebufferResized = false;
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS)
	{
		LOG_CRITICAL("Failed to present frame");
		throw;
	}
}

VkFormat Swapchain::getFormat()
{
	return m_format;
}

VkExtent2D Swapchain::getExtent()
{
	return m_extent;
}

VkFramebuffer Swapchain::getFramebuffer(uint32_t index)
{
	return m_framebuffers[index];
}

void Swapchain::cleanup()
{
	LOG_INFO("Destroying swapchain");

	// Framebuffers
	for (auto framebuffer : m_framebuffers)
		vkDestroyFramebuffer(m_device->getLogical(), framebuffer, nullptr);

	// Image Views
	for (auto imageView : m_imageViews)
		vkDestroyImageView(m_device->getLogical(), imageView, nullptr);

	// Swapchain
	vkDestroySwapchainKHR(m_device->getLogical(), m_swapchain, nullptr);

	// Don't desctroy syncronization object during recreation
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
	LOG_INFO("Initializing swapchain");

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
	LOG_TRACE("Looking for image format VK_FORMAT_B8G8R8A8_SRGB with VK_COLOR_SPACE_SRGB_NONLINEAR_KHR");

	// Find a format for RGBA8 and SRGB Nonlinear
	for (const auto& availableFormat : availableFormats)
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;

	LOG_TRACE("Image format not found. Use backup format");

	// Return the first format as a backup
	return availableFormats[0];
}

VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	LOG_TRACE("Looking for present mode VK_PRESENT_MODE_MAILBOX_KHR");

	// Find mailbox present mode
	for (const auto& availablePresentMode : availablePresentModes)
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;

	LOG_TRACE("Present mode not found. Using backup VK_PRESENT_MODE_FIFO_KHR");

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