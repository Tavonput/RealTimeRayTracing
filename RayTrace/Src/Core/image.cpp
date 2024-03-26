#include "pch.h"

#include "image.h"

Image Image::CreateImage(CreateInfo& createInfo)
{
    APP_LOG_INFO("Create image ({})", createInfo.name);

    Image image;
    image.m_name = createInfo.name;

    // Create image
    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = createInfo.width;
    imageInfo.extent.height = createInfo.height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = createInfo.mipLevels;
    imageInfo.arrayLayers   = createInfo.layerCount;
    imageInfo.format        = createInfo.format;
    imageInfo.tiling        = createInfo.tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = createInfo.usage;
    imageInfo.samples       = createInfo.numSamples;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

    if (createInfo.layerCount == 6)
        imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

    if (vkCreateImage(createInfo.device->getLogical(), &imageInfo, nullptr, &image.image) != VK_SUCCESS)
    {
        APP_LOG_CRITICAL("Failed to create image ({})", createInfo.name);
        throw;
    }

    // Allocate memory
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(createInfo.device->getLogical(), image.image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = Device::findMemoryType(memRequirements.memoryTypeBits, createInfo.properties, createInfo.device->getPhysical());

    if (vkAllocateMemory(createInfo.device->getLogical(), &allocInfo, nullptr, &image.memory) != VK_SUCCESS)
    {
        APP_LOG_CRITICAL("Failed allocate image memory ({})", createInfo.name);
        throw;
    }

    // Bind image memory
    vkBindImageMemory(createInfo.device->getLogical(), image.image, image.memory, 0);

    image.format = createInfo.format;
    image.numSamples = createInfo.numSamples;

    return image;
}

void Image::SetupImageView(Image& image, ImageViewSetupInfo& info)
{
    // Setup image view
    VkImageViewCreateInfo createInfo{};
    createInfo.sType  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image  = image.image;
    createInfo.format = info.format;

    if (info.layerCount == 6)
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    else
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask     = info.aspectFlags;
    createInfo.subresourceRange.baseMipLevel   = 0;
    createInfo.subresourceRange.levelCount     = info.mipLevels;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount     = info.layerCount;

    if (vkCreateImageView(info.device->getLogical(), &createInfo, nullptr, &image.view) != VK_SUCCESS)
    {
        APP_LOG_CRITICAL("Failed to create image view");
        throw;
    }
}

void Image::TransitionImage(VkCommandBuffer cmdBuf, VkImage image, TransitionInfo& info)
{
    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.oldLayout           = info.oldLayout;
    imageMemoryBarrier.newLayout           = info.newLayout;
    imageMemoryBarrier.image               = image;
    imageMemoryBarrier.srcAccessMask       = info.srcAccessMask;
    imageMemoryBarrier.dstAccessMask       = info.dstAccessMask;
    imageMemoryBarrier.srcQueueFamilyIndex = info.srcQueueFamilyIndex;
    imageMemoryBarrier.dstQueueFamilyIndex = info.dstQueueFamilyIndex;

    imageMemoryBarrier.subresourceRange.aspectMask     = info.aspectMask;
    imageMemoryBarrier.subresourceRange.levelCount     = info.levelCount;
    imageMemoryBarrier.subresourceRange.layerCount     = info.layerCount;
    imageMemoryBarrier.subresourceRange.baseMipLevel   = info.baseMipLevel;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = info.baseArrayLayer;

    vkCmdPipelineBarrier(
        cmdBuf, 
        info.srcStageMask, info.dstStageMask, 
        0, 
        0, nullptr, 
        0, nullptr, 
        1, &imageMemoryBarrier);
}

void Image::CopyFromBuffer(VkCommandBuffer cmdBuf, VkImage image, VkBuffer buffer, uint32_t width, uint32_t height, VkImageSubresourceLayers subresource)
{
    VkBufferImageCopy region{};
    region.bufferOffset      = 0;
    region.bufferRowLength   = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource  = subresource;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };

    vkCmdCopyBufferToImage(
        cmdBuf,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
}

VkImageMemoryBarrier Image::CreateImageMemoryBarrier(VkImage image)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.image               = image;
    barrier.srcAccessMask       = 0;
    barrier.dstAccessMask       = 0;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.layerCount     = 1;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.baseArrayLayer = 0;

    return barrier;
}

void Image::cleanup(const VkDevice& device)
{
    APP_LOG_INFO("Destroying image ({})", m_name);

    if (image != VK_NULL_HANDLE)
        vkDestroyImage(device, image, nullptr);

    if (view != VK_NULL_HANDLE)
        vkDestroyImageView(device, view, nullptr);

    if (memory != VK_NULL_HANDLE)
        vkFreeMemory(device, memory, nullptr);
}
