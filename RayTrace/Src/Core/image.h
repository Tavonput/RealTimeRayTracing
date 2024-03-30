#pragma once

#include "device.h"

class Image
{
public:
	VkImage        image  = VK_NULL_HANDLE;
	VkImageView    view   = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;

	VkFormat              format     = VK_FORMAT_UNDEFINED;
	VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;

	std::string m_name = "";

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
		const char*           name       = "";
	};

	struct ImageViewSetupInfo
	{
		VkFormat           format      = VK_FORMAT_UNDEFINED;
		VkImageAspectFlags aspectFlags = 0;
		uint32_t           mipLevels   = 0;
		uint32_t           layerCount  = 0;
		const Device*      device       = nullptr;
	};

	struct TransitionInfo
	{
		VkImageAspectFlags aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		uint32_t           baseMipLevel   = 0;
		uint32_t           levelCount     = 1;
		uint32_t           baseArrayLayer = 0;
		uint32_t           layerCount     = 1;

		const void*     pNext               = nullptr;
		VkAccessFlags   srcAccessMask       = 0;
		VkAccessFlags   dstAccessMask       = 0;
		VkImageLayout   oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageLayout   newLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
		uint32_t        srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		uint32_t        dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	};

	static Image CreateImage(CreateInfo& createInfo);

	static void SetupImageView(Image& image, ImageViewSetupInfo& info);

	static void TransitionImage(VkCommandBuffer cmdBuf, VkImage image, TransitionInfo& info);

	static void CopyFromBuffer(
		VkCommandBuffer          cmdBuf, 
		VkImage                  image, 
		VkBuffer                 buffer, 
		uint32_t                 width, 
		uint32_t                 height, 
		VkImageSubresourceLayers subresource);

	/**
	 * Create a default image memory barrier.
	 * 
	 * @param image: The VkImage to create the barrier on.
	 * 
	 * @return A default image memory barrier.
	 */
	static VkImageMemoryBarrier CreateImageMemoryBarrier(VkImage image);

	void cleanup(const VkDevice& device);
};
