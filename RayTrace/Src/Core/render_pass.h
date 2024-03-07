#pragma once

#include "Application/logging.h"
#include "device.h"

class RenderPass
{
public:

	// All render passes
	enum PassType
	{
		MAIN = 0,
		POST = 1
	};

	// Builder Class
	class Builder
	{
	public:
		Builder(const Device& device);

		RenderPass buildPass(const std::string name);
		std::vector<VkClearValue> getClearValues();
		void reset();

		void addColorAttachment(
			VkFormat format,
			VkSampleCountFlagBits numSamples,
			VkImageLayout initialLayout,
			VkImageLayout finalLayout,
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

	// Render Pass Class
	VkRenderPass              renderPass;
	std::vector<VkClearValue> clearValues;

	std::string name = "";

	RenderPass(VkRenderPass _renderPass, std::vector<VkClearValue> _clearValues, const std::string _name)
		: renderPass(_renderPass), clearValues(_clearValues), name(_name) {}

	void cleanup(const Device& device);
};
