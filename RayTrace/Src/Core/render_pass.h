#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Application/logging.h"
#include "device.h"

namespace RenderPass {

	class Builder
	{
	public:
		void init(const Device& device, Logger logger);

		VkRenderPass build();
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
		const Device* m_device;

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

	class Manager
	{
	public:
		void init(const Device& device, Logger logger);

		void addPass(VkRenderPass renderPass);

		VkRenderPass& getPass(uint32_t index);

		void cleanup();

	private:
		const Device* m_device;

		Logger m_logger;

		std::vector<VkRenderPass> m_passes;
	};

} // namespace

