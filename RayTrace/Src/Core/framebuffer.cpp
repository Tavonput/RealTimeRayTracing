#include "pch.h"
#include "framebuffer.h"

Framebuffer::Framebuffer(Framebuffer::CreateInfo info)
{
	APP_LOG_INFO("Creating framebuffer ({})", info.name);

	m_device = info.pDevice;
	m_name   = info.name;

	// Framebuffer
	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass      = info.pRenderPass->renderPass;
	framebufferInfo.attachmentCount = info.numAttachments;
	framebufferInfo.pAttachments    = info.pAttachments;
	framebufferInfo.width           = info.extent.width;
	framebufferInfo.height          = info.extent.height;
	framebufferInfo.layers          = 1;

	if (vkCreateFramebuffer(m_device->getLogical(), &framebufferInfo, nullptr, &m_framebuffer) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create framebuffer ({})", info.name);
		throw;
	}
}

void Framebuffer::cleanup()
{
	APP_LOG_INFO("Destroying framebuffer ({})", m_name);

	vkDestroyFramebuffer(m_device->getLogical(), m_framebuffer, nullptr);
}
