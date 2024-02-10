#pragma once

#include "device.h"

class Image
{
public:
	VkImage        image  = VK_NULL_HANDLE;
	VkImageView    view   = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;

	struct CreateInfo
	{
		uint32_t              width      = 0;
		uint32_t              height     = 0;
		uint32_t              mipLevels  = 0;
		uint32_t              layerCount = 0;
		VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;
		VkFormat              format     = VK_FORMAT_UNDEFINED;
		VkImageTiling         tiling     = VK_IMAGE_TILING_OPTIMAL;
		VkImageUsageFlags     usage      = 0;
		VkMemoryPropertyFlags properties = 0;
		const Device*         device     = nullptr;
	};

	struct ImageViewSetupInfo
	{
		VkFormat           format      = VK_FORMAT_UNDEFINED;
		VkImageAspectFlags aspectFlags = 0;
		uint32_t           mipLevels   = 0;
		uint32_t           layerCount  = 0;
		const Device*      device       = nullptr;
	};

	static Image CreateImage(CreateInfo& createInfo);

	static void SetupImageView(Image& image, ImageViewSetupInfo& info);

	void cleanup(const VkDevice& device);
};