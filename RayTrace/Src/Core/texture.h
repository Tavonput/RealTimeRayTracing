#pragma once

#include "Application/logging.h"

#include "Utils/stb_image_usage.h"

#include "device.h"
#include "command.h"
#include "image.h"

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
	static Texture CreateFromFile(Texture::CreateInfo& info);

	const Image& getImage() const { return m_image; }
	const VkDescriptorImageInfo& getDescriptor() const { return m_descriptor; }

	void cleanup();

private:
	const Device* m_device = nullptr;
	std::string   m_name = "";

	Image                 m_image;
	VkDescriptorImageInfo m_descriptor{};

	Texture(Texture::CreateInfo& info);

	void transitionImage(VkCommandBuffer cmdBuf);
};
