#include "render_pass.h"

// -----------------------------------------------------
// -----------------------------------------------------
// Builder
// -----------------------------------------------------
// -----------------------------------------------------
void RenderPass::Builder::init(const Device& device, Logger logger)
{
	m_device = &device;
	m_logger = logger;
}

VkRenderPass RenderPass::Builder::build()
{
	// Subpass description
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	if (m_usingColor)
	{
		subpass.colorAttachmentCount = static_cast<uint32_t>(m_colorAttachments.size());
		subpass.pColorAttachments    = m_colorAttachmentRefs.data();
	}

	if (m_usingDepth)
		subpass.pDepthStencilAttachment = &m_depthAttachmentRef;

	if (m_usingResolve)
		subpass.pResolveAttachments = &m_resolveAttachmentRef;

	// Subpass dependency
	VkSubpassDependency dependency{};
	dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass    = 0;
	dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	// Create render pass
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(m_attachments.size());
	renderPassInfo.pAttachments    = m_attachments.data();
	renderPassInfo.subpassCount    = 1;
	renderPassInfo.pSubpasses      = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies   = &dependency;

	VkRenderPass renderPass;
	if (vkCreateRenderPass(m_device->getLogical(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		LOG_CRITICAL("Failed to create render pass");
		throw;
	}

	LOG_INFO("Render pass build successful");

	return renderPass;
}

void RenderPass::Builder::reset()
{
	// Color
	m_usingColor = false;
	m_colorAttachments = std::vector<VkAttachmentDescription>();
	m_colorAttachmentRefs = std::vector<VkAttachmentReference>();

	// Depth
	m_usingDepth = false;
	m_depthAttachment = VkAttachmentDescription{};
	m_depthAttachmentRef = VkAttachmentReference{};

	// Resolve
	m_usingResolve = false;
	m_resolveAttachment = VkAttachmentDescription{};
	m_resolveAttachmentRef = VkAttachmentReference{};

	// Attachments and clear colors
	m_attachments = std::vector<VkAttachmentDescription>();
	m_clearValues = std::vector<VkClearValue>();
}

void RenderPass::Builder::addColorAttachment(VkFormat format, VkSampleCountFlagBits numSamples, VkImageLayout initialLayout, VkClearValue clearValue, bool present)
{
	// User is using a color attachment
	if (!m_usingColor)
		m_usingColor = true;

	// Attachment
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format         = format;
	colorAttachment.samples        = numSamples;
	colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout  = initialLayout;
	colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	if (present)
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Reference
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = static_cast<uint32_t>(m_attachments.size());
	colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Store
	m_colorAttachments.push_back(colorAttachment);
	m_colorAttachmentRefs.push_back(colorAttachmentRef);

	m_attachments.push_back(colorAttachment);

	m_clearValues.push_back(clearValue);
}

void RenderPass::Builder::addDepthAttachment(VkFormat format, VkSampleCountFlagBits numSamples, VkImageLayout initialLayout, VkClearValue clearValue)
{
	// User is using a depth attachment
	if (!m_usingDepth)
		m_usingDepth = true;

	// Attachment
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format         = format;
	depthAttachment.samples        = numSamples;
	depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout  = initialLayout;
	depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Reference
	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = static_cast<uint32_t>(m_attachments.size());
	depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Store
	m_depthAttachment = depthAttachment;
	m_depthAttachmentRef = depthAttachmentRef;

	m_attachments.push_back(depthAttachment);

	m_clearValues.push_back(clearValue);
}

void RenderPass::Builder::addResolveAttachment(VkFormat format, VkImageLayout initialLayout, VkClearValue clearValue)
{
	// User is using a resolve attachment
	if (!m_usingResolve)
		m_usingResolve = true;

	// Attachment
	VkAttachmentDescription resolveAttachment{};
	resolveAttachment.format         = format;
	resolveAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	resolveAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	resolveAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	resolveAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	resolveAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	resolveAttachment.initialLayout  = initialLayout;
	resolveAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Reference
	VkAttachmentReference resolveAttachmentRef{};
	resolveAttachmentRef.attachment = static_cast<uint32_t>(m_attachments.size());
	resolveAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Store
	m_resolveAttachment = resolveAttachment;
	m_resolveAttachmentRef = resolveAttachmentRef;

	m_attachments.push_back(resolveAttachment);

	m_clearValues.push_back(clearValue);
}

// -----------------------------------------------------
// -----------------------------------------------------
// Manager
// -----------------------------------------------------
// -----------------------------------------------------

void RenderPass::Manager::init(const Device& device, Logger logger)
{
	m_device = &device;
	m_logger = logger;
}

void RenderPass::Manager::addPass(VkRenderPass renderPass)
{
	m_passes.push_back(renderPass);
}

VkRenderPass& RenderPass::Manager::getPass(uint32_t index)
{
	return m_passes[index];
}

void RenderPass::Manager::cleanup()
{
	LOG_INFO("Destroying render passes");

	for (auto& pass : m_passes)
		vkDestroyRenderPass(m_device->getLogical(), pass, nullptr);
}
