#pragma once

#include "Application/logging.h"

#include "render_pass.h"
#include "device.h"
#include "image.h"

class Framebuffer
{
public:
	struct CreateInfo
	{
		const RenderPass* pRenderPass    = nullptr;
		VkImageView*      pAttachments   = nullptr;
		uint8_t           numAttachments = 0;
		VkExtent2D        extent;
		const Device*     pDevice        = nullptr;
		const char*       name = "";
	};

	Framebuffer() = default;
	Framebuffer(Framebuffer::CreateInfo info);

	const VkFramebuffer get() const { return m_framebuffer; }

	void cleanup();

private:
	const Device* m_device = nullptr;
	std::string   m_name = "";

	VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
};
