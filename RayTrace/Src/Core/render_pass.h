#pragma once

#include "Application/logging.h"
#include "device.h"

class RenderPass
{
public:

	// Builder Class
	class Builder
	{
	public:
		Builder(const Device& device, Logger logger);

		RenderPass buildPass();
		std::vector<VkClearValue> getClearValues();
		void reset();

		void addColorAttachment(
			VkFormat format,
			VkSampleCountFlagBits numSamples,
			VkImageLayout initialLayout,
			VkClearValue clearValue,
			bool present = false);

		void addDepthAttachment(
			VkFormat format,
			VkSampleCountFlagBits numSamples,
			VkImageLayout initialLayout,
			VkClearValue clearValue);

		void addResolveAttachment(
			VkFormat format,
			VkImageLayout initialLayout,
			VkClearValue clearValue);

	private:
		const Device* m_device = nullptr;

		Logger m_logger;

		// Color
		bool m_usingColor = false;
		std::vector<VkAttachmentDescription> m_colorAttachments;
		std::vector<VkAttachmentReference>   m_colorAttachmentRefs;

		// Depth
		bool m_usingDepth = false;
		VkAttachmentDescription m_depthAttachment{};
		VkAttachmentReference   m_depthAttachmentRef{};

		// Resolve
		bool m_usingResolve = false;
		VkAttachmentDescription m_resolveAttachment{};
		VkAttachmentReference   m_resolveAttachmentRef{};

		std::vector<VkAttachmentDescription> m_attachments;
		std::vector<VkClearValue>            m_clearValues;
	};

	// Manager Class
	class Manager
	{
	public:
		void init(const Device& device, Logger logger);

		void addPass(RenderPass renderPass);
		void beginPass(uint32_t index, VkFramebuffer framebuffer, VkExtent2D extent, VkCommandBuffer commandBuffer);

		VkRenderPass& getPass(uint32_t index);

		void cleanup();

	private:
		const Device* m_device;
		Logger        m_logger;

		std::vector<RenderPass> m_passes;
	};

	// Render Pass Class
	VkRenderPass              renderPass;
	std::vector<VkClearValue> clearValues;

	RenderPass(VkRenderPass _renderPass, std::vector<VkClearValue> _clearValues)
		: renderPass(_renderPass), clearValues(_clearValues) {}
};
