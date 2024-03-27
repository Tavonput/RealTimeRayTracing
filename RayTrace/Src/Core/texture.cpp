#include "pch.h"
#include "texture.h"

Texture Texture::Create(Texture::CreateInfo& info)
{
	return Texture(info);
}

std::vector<Texture> Texture::CreateBatch(std::vector<Texture::CreateInfo>& infos, uint32_t numTextures)
{
	std::vector<Texture> textures;
	textures.reserve(numTextures);

	std::vector<VkBuffer>       stagingBuffers(numTextures);
	std::vector<VkDeviceMemory> stagingMemory(numTextures);

	const Device*        device = infos[0].pDevice;
	const CommandSystem* cmdSys = infos[0].pCommandSystem;

	// Sampler
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(device->getPhysical(), &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter               = VK_FILTER_LINEAR;
	samplerInfo.minFilter               = VK_FILTER_LINEAR;
	samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable        = VK_TRUE;
	samplerInfo.maxAnisotropy           = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable           = VK_FALSE;
	samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.minLod                  = 0.0f;
	samplerInfo.maxLod                  = 1.0f;
	samplerInfo.mipLodBias              = 0.0f;

	VkCommandBuffer cmdBuf = cmdSys->beginSingleTimeCommands();

	for (uint32_t i = 0; i < numTextures; i++)
	{
		// Load texture
		char* pixels;
		int width, height, channels;
		Texture::LoadTexture(infos[i].filename, infos[i].fileType, &width, &height, &channels, &pixels);

		// Setup type specific parameters
		VkFormat      format      = VK_FORMAT_UNDEFINED;
		size_t        channelSize = sizeof uint8_t;
		VkImageTiling imageTiling = VK_IMAGE_TILING_OPTIMAL;
		uint32_t      mipLevels   = 1;
		switch (infos[i].fileType)
		{
			case FileType::ALBEDO:
				format    = VK_FORMAT_R8G8B8A8_SRGB;
				mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
				break;

			case FileType::NORMAL:
				format    = VK_FORMAT_R8G8B8A8_UNORM;
				mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
				break;

			case FileType::NONE:
				APP_LOG_CRITICAL("Invalid file type: NONE");
				break;

			default:
				APP_LOG_CRITICAL("No file type was specified");
		}

		VkDeviceSize imageSize = static_cast<uint64_t>(width) * height * channels * channelSize;

		// Create staging buffer
		Buffer::CreateBuffer(
			imageSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffers[i], stagingMemory[i],
			*device);

		// Transfer buffer data into staging buffer memory
		void* deviceData;
		vkMapMemory(device->getLogical(), stagingMemory[i], 0, imageSize, 0, &deviceData);
		memcpy(deviceData, pixels, (size_t)imageSize);
		vkUnmapMemory(device->getLogical(), stagingMemory[i]);

		// Create image
		Image::CreateInfo imgCreateInfo{};
		imgCreateInfo.width      = width;
		imgCreateInfo.height     = height;
		imgCreateInfo.mipLevels  = mipLevels;
		imgCreateInfo.layerCount = 1;
		imgCreateInfo.numSamples = VK_SAMPLE_COUNT_1_BIT;
		imgCreateInfo.tiling     = VK_IMAGE_TILING_OPTIMAL;
		imgCreateInfo.usage      = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imgCreateInfo.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		imgCreateInfo.format     = format;
		imgCreateInfo.device     = device;
		imgCreateInfo.name       = infos[i].name;

		Image image = Image::CreateImage(imgCreateInfo);

		// Setup image view
		Image::ImageViewSetupInfo viewSetupInfo{};
		viewSetupInfo.format      = imgCreateInfo.format;
		viewSetupInfo.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		viewSetupInfo.mipLevels   = mipLevels;
		viewSetupInfo.layerCount  = 1;
		viewSetupInfo.device      = device;
		Image::SetupImageView(image, viewSetupInfo);
		
		// Transition image layout for receiving data
		Image::TransitionInfo transitionInfo{};
		transitionInfo.oldLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
		transitionInfo.newLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		transitionInfo.srcStageMask  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		transitionInfo.srcAccessMask = 0;
		transitionInfo.dstStageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT;
		transitionInfo.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		transitionInfo.levelCount    = mipLevels;
		Image::TransitionImage(cmdBuf, image.image, transitionInfo);

		// Copy data from staging buffer to image
		Image::CopyFromBuffer(
			cmdBuf,
			image.image, stagingBuffers[i],
			width, height,
			{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 });

		// Generate mip maps and transition layout to shader read only optimal
		Texture::GenerateMipMaps(cmdBuf, image.image, width, height, mipLevels);

		// Setup descriptor information
		VkDescriptorImageInfo descriptor{};
		descriptor.imageView   = image.view;
		descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		if (vkCreateSampler(device->getLogical(), &samplerInfo, nullptr, &descriptor.sampler) != VK_SUCCESS)
		{
			APP_LOG_CRITICAL("Failed to create sampler ({})", infos[i].name);
			throw;
		}

		// Store texture
		textures.emplace_back(Texture(infos[i].pDevice, infos[i].name, image, descriptor));
	}

	infos[0].pCommandSystem->endSingleTimeCommands(cmdBuf, infos[0].pDevice->getGraphicsQueue());

	// Cleanup staging resources
	for (uint32_t i = 0; i < numTextures; i++)
	{
		vkDestroyBuffer(device->getLogical(), stagingBuffers[i], nullptr);
		vkFreeMemory(device->getLogical(), stagingMemory[i], nullptr);
	}

	return textures;
}

void Texture::cleanup()
{
	APP_LOG_INFO("Destroying texture ({})", m_name);

	m_image.cleanup(m_device->getLogical());
	vkDestroySampler(m_device->getLogical(), m_descriptor.sampler, nullptr);
}

Texture::Texture(Texture::CreateInfo& info)
{
	APP_LOG_INFO("Creating texture ({})", info.name);

	m_device = info.pDevice;
	m_name   = info.name;

	// Create image
	Image::CreateInfo imgCreateInfo{};
	imgCreateInfo.width      = info.extent.width;
	imgCreateInfo.height     = info.extent.height;
	imgCreateInfo.mipLevels  = 1;
	imgCreateInfo.layerCount = 1;
	imgCreateInfo.numSamples = VK_SAMPLE_COUNT_1_BIT;
	imgCreateInfo.tiling     = VK_IMAGE_TILING_OPTIMAL;
	imgCreateInfo.usage      = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
	imgCreateInfo.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	imgCreateInfo.device     = info.pDevice;
	imgCreateInfo.name       = info.name;

	imgCreateInfo.format = info.pDevice->findSupportedFormat(
		{ VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R16G16B16A16_SFLOAT, VK_FORMAT_R8G8B8A8_UNORM },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

	m_image = Image::CreateImage(imgCreateInfo);

	// Transition image
	VkCommandBuffer cmdBuf = info.pCommandSystem->beginSingleTimeCommands();
	
	Image::TransitionInfo transitionInfo{};
	transitionInfo.oldLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
	transitionInfo.newLayout     = VK_IMAGE_LAYOUT_GENERAL;
	transitionInfo.srcAccessMask = 0;
	transitionInfo.srcStageMask  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	transitionInfo.dstAccessMask = 0;
	transitionInfo.dstStageMask  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

	Image::TransitionImage(cmdBuf, m_image.image, transitionInfo);

	m_descriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	info.pCommandSystem->endSingleTimeCommands(cmdBuf, info.pDevice->getGraphicsQueue());

	// Setup image view
	Image::ImageViewSetupInfo viewSetupInfo{};
	viewSetupInfo.format      = imgCreateInfo.format;
	viewSetupInfo.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	viewSetupInfo.mipLevels   = 1;
	viewSetupInfo.layerCount  = 1;
	viewSetupInfo.device      = info.pDevice;

	Image::SetupImageView(m_image, viewSetupInfo);
	m_descriptor.imageView = m_image.view;

	// Create sampler
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(info.pDevice->getPhysical(), &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter               = VK_FILTER_LINEAR;
	samplerInfo.minFilter               = VK_FILTER_LINEAR;
	samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable        = VK_TRUE;
	samplerInfo.maxAnisotropy           = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable           = VK_FALSE;
	samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.minLod                  = 0.0f;
	samplerInfo.maxLod                  = 1.0f;
	samplerInfo.mipLodBias              = 0.0f;

	if (vkCreateSampler(info.pDevice->getLogical(), &samplerInfo, nullptr, &m_descriptor.sampler) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create sampler ({})", m_name);
		throw;
	}
}

void Texture::LoadTexture(const char* file, FileType type, int* width, int* height, int* channels, char** data)
{
	APP_LOG_TRACE("Loading texture {}", file);

	int loadFormat = 0;

	switch (type)
	{
		case FileType::ALBEDO:
		case FileType::NORMAL:
			loadFormat = STBI_rgb_alpha;
			break;

		case FileType::NONE:
			APP_LOG_CRITICAL("File type is NONE");
			break;

		default:
			APP_LOG_CRITICAL("No file type was specified");
	}

	stbi_set_flip_vertically_on_load(true);
	*data = (char*)stbi_load(file, width, height, channels, loadFormat);
	if (!*data)
	{
		APP_LOG_CRITICAL("Failed to load texture");
		throw std::exception();
	}

	// Force rgb textures to be rgba
	if (*channels == 3)
		*channels = 4;
}

void Texture::GenerateMipMaps(VkCommandBuffer cmdBuf, VkImage image, uint32_t width, uint32_t height, uint32_t mipLevels)
{
	// Inital barrier setup
	VkImageMemoryBarrier barrier = Image::CreateImageMemoryBarrier(image);

	int32_t mipWidth  = width;
	int32_t mipHeight = height;

	for (uint32_t i = 1; i < mipLevels; i++)
	{
		// Transition the image to be blitted to transfer source optimal
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout                     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask                 = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(
			cmdBuf,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		// Blit the image
		VkImageBlit blit{};
		blit.srcOffsets[0]                 = { 0, 0, 0 };
		blit.srcOffsets[1]                 = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel       = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount     = 1;

		blit.dstOffsets[0]                 = { 0, 0, 0 };
		blit.dstOffsets[1]                 = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel       = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount     = 1;

		vkCmdBlitImage(
			cmdBuf,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);

		// Transition the image to be blitted to shader read only optimal
		barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(
			cmdBuf,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	// Transition the last image to shader read only optimal
	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(
		cmdBuf,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);
}
