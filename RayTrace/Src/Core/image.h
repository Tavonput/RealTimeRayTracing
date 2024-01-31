#pragma once

#include <vulkan/vulkan.h>

#include "device.h"

class Image
{
public:
	static void setupImageView(
		VkImageView& imageView,
		VkImage& image,
		VkFormat& format,
		VkImageAspectFlags aspectFlags,
		uint32_t mipLevels,
		uint32_t layerCount,
		const VkDevice& device);
};