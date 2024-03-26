#pragma once

#include "Application/logging.h"

#include "Utils/stb_image_usage.h"

#include "device.h"
#include "command.h"
#include "image.h"
#include "buffer.h"

class Texture
{
public:
	struct CreateInfo 
	{
		const Device*        pDevice        = nullptr;
		const CommandSystem* pCommandSystem = nullptr;
		VkExtent2D           extent         = { 0, 0 };
		const char*          filename       = nullptr;
		const char*          name           = "";
	};

	Texture() = default;

	static Texture Create(Texture::CreateInfo& info);
	static std::vector<Texture> CreateBatch(std::vector<Texture::CreateInfo>& infos, uint32_t numTextures);

	static void LoadTexture(const char* file, int* width, int* height, int* channels, char** data);
	static void GenerateMipMaps(VkCommandBuffer cmdBuf, VkImage image, uint32_t width, uint32_t height, uint32_t mipLevels);

	const Image& getImage() const { return m_image; }
	const VkDescriptorImageInfo& getDescriptor() const { return m_descriptor; }

	void cleanup();

private:
	const Device* m_device = nullptr;
	std::string   m_name = "";

	Image                 m_image;
	VkDescriptorImageInfo m_descriptor{};

	Texture(Texture::CreateInfo& info);

	Texture(const Device* device, std::string name, Image image, VkDescriptorImageInfo descriptor)
		: m_device(device), m_name(name), m_image(image), m_descriptor(descriptor) {}
};
