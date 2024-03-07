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
