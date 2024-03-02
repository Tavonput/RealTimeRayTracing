#include "pch.h"

#include "depth_buffer.h"

DepthBuffer::DepthBuffer(const Device& device, VkExtent2D extent, VkSampleCountFlagBits numSamples, const std::string name)
{
	APP_LOG_INFO("Creating depth buffer ({})", name);

	m_device = &device;
	m_name = name;

	// Find format
	format = m_device->findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

	// Create image
	Image::CreateInfo imgCreateInfo{};
	imgCreateInfo.width      = extent.width;
	imgCreateInfo.height     = extent.height;
	imgCreateInfo.mipLevels  = 1;
	imgCreateInfo.layerCount = 1;
	imgCreateInfo.numSamples = numSamples;
	imgCreateInfo.format     = format;
	imgCreateInfo.tiling     = VK_IMAGE_TILING_OPTIMAL;
	imgCreateInfo.usage      = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imgCreateInfo.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	imgCreateInfo.device     = m_device;
	imgCreateInfo.name       = name.c_str();

	image = Image::CreateImage(imgCreateInfo);

	// Setup image view
	Image::ImageViewSetupInfo viewSetupInfo{};
	viewSetupInfo.format      = format;
	viewSetupInfo.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
	viewSetupInfo.mipLevels   = 1;
	viewSetupInfo.layerCount  = 1;
	viewSetupInfo.device      = m_device;

	Image::SetupImageView(image, viewSetupInfo);
}

void DepthBuffer::cleanup()
{
	APP_LOG_INFO("Destroying depth buffer ({})", m_name);

	image.cleanup(m_device->getLogical());
}
