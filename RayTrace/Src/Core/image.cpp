#include "pch.h"

#include "image.h"

void Image::setupImageView(
	VkImageView& imageView, 
	VkImage& image, 
	VkFormat& format, 
	VkImageAspectFlags aspectFlags, 
	uint32_t mipLevels, 
	uint32_t layerCount, 
	const VkDevice& device)
{
    // Setup image view
    VkImageViewCreateInfo createInfo{};
    createInfo.sType  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image  = image;
    createInfo.format = format;

    if (layerCount == 6)
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    else
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask     = aspectFlags;
    createInfo.subresourceRange.baseMipLevel   = 0;
    createInfo.subresourceRange.levelCount     = mipLevels;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount     = layerCount;

    if (vkCreateImageView(device, &createInfo, nullptr, &imageView) != VK_SUCCESS)
        throw std::runtime_error("failed to create image views!");
}
