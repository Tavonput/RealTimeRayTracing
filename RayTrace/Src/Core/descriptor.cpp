#include "pch.h"
#include "descriptor.h"

// --------------------------------------------------------------------------
// Descriptor Set Layout
//

DescriptorSetLayout DescriptorSetLayout::Builder::buildLayout(const std::string name)
{
	APP_LOG_INFO("Building descriptor set layout ({})", name);

	VkDescriptorSetLayoutCreateInfo info{};
	info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.bindingCount = static_cast<uint32_t>(m_bindings.size());
	info.pBindings    = m_bindings.data();

	VkDescriptorSetLayout layout;
	if (vkCreateDescriptorSetLayout(m_device->getLogical(), &info, nullptr, &layout) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create descriptor set layout ({})", name);
		throw;
	};

	return DescriptorSetLayout(layout, m_bindings, static_cast<uint32_t>(m_bindings.size()), name);
}

void DescriptorSetLayout::Builder::reset()
{
	m_bindings = std::vector<VkDescriptorSetLayoutBinding>{};
}

void DescriptorSetLayout::Builder::addBinding(
	uint32_t           binding, 
	VkDescriptorType   descriptorType, 
	uint32_t           descriptorCount, 
	VkShaderStageFlags stageFlags, 
	VkSampler*         pImmutableSamplers) // nullptr
{
	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding            = static_cast<uint32_t>(binding);
	layoutBinding.descriptorType     = descriptorType;
	layoutBinding.descriptorCount    = descriptorCount;
	layoutBinding.stageFlags         = stageFlags;
	layoutBinding.pImmutableSamplers = pImmutableSamplers;

	m_bindings.push_back(layoutBinding);
}

void DescriptorSetLayout::cleanup(const Device& device)
{
	APP_LOG_INFO("Destroying descriptor set layout ({})", m_name);

	vkDestroyDescriptorSetLayout(device.getLogical(), layout, nullptr);
}

// --------------------------------------------------------------------------
// Descriptor Set
//

void DescriptorSet::addBufferWrite(Buffer buffer, BufferType type, VkDeviceSize offset, uint32_t binding)
{
	// Write info
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = buffer.getBuffer();
	bufferInfo.offset = offset;
	bufferInfo.range  = buffer.getSize();

	m_writeBufferInfos.emplace_back(bufferInfo);

	// Descriptor write
	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet          = m_set;
	descriptorWrite.dstBinding      = static_cast<uint32_t>(binding);
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;

	// Set the correct descriptor type
	switch (type)
	{
	case BufferType::UNIFORM: 
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; 
		break;

	case BufferType::STORAGE: 
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; 
		break;

	default:
		APP_LOG_CRITICAL("Invalid buffer type for descriptor set");
		throw;
	}

	m_descriptorWrites.emplace_back(descriptorWrite);
}

void DescriptorSet::addImageWrite(VkImageView imageView, VkSampler sampler, uint32_t binding)
{
	// Write info
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView   = imageView;
	imageInfo.sampler     = sampler;

	m_writeImageInfos.push_back(imageInfo);

	// Descriptor write
	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet          = m_set;
	descriptorWrite.dstBinding      = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;

	m_descriptorWrites.push_back(descriptorWrite);
}

void DescriptorSet::update(const Device& device)
{
	// Setup infos for each write
	uint32_t bufferCounter = 0;
	uint32_t imageCounter  = 0;
	for (uint32_t i = 0; i < m_descriptorWrites.size(); i++)
	{
		// Figure out if we have an image or a buffer
		if (m_descriptorWrites[i].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
		{
			m_descriptorWrites[i].pImageInfo = &m_writeImageInfos[imageCounter];
			imageCounter++;
		}
		else
		{
			m_descriptorWrites[i].pBufferInfo = &m_writeBufferInfos[bufferCounter];
			bufferCounter++;
		}
	}

	// Update
	vkUpdateDescriptorSets(device.getLogical(), static_cast<uint32_t>(m_descriptorWrites.size()), m_descriptorWrites.data(), 0, nullptr);

	// Reset writes
	m_descriptorWrites = std::vector<VkWriteDescriptorSet>{};
	m_writeBufferInfos = std::vector<VkDescriptorBufferInfo>{};
	m_writeImageInfos  = std::vector<VkDescriptorImageInfo>{};
}

VkDescriptorSet& DescriptorSet::getSet()
{
	return m_set;
}

// --------------------------------------------------------------------------
// Descriptor Pool
//

void DescriptorPool::init(const Device& device, uint32_t framesInFlight, uint32_t setsPerFrame)
{
	APP_LOG_INFO("Initializing descriptor pool");

	m_device = &device;

	// Pool sizes
	std::array<VkDescriptorPoolSize, 3> poolSizes{};

	poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = framesInFlight;

	poolSizes[1].type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[1].descriptorCount = framesInFlight;

	poolSizes[2].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[2].descriptorCount = framesInFlight;

	// Descriptor pool creation
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes    = poolSizes.data();
	poolInfo.maxSets       = setsPerFrame * framesInFlight;

	if (vkCreateDescriptorPool(m_device->getLogical(), &poolInfo, nullptr, &m_pool) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create descriptor pool");
		throw;
	}

	std::array<VkDescriptorPoolSize, 1> imguiPoolSizes = {}; //Might need adjustment later as GUI advances. 
	imguiPoolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	imguiPoolSizes[0].descriptorCount = 1;

	VkDescriptorPoolCreateInfo imguiPoolInfo = {};
	imguiPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	imguiPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	imguiPoolInfo.maxSets = 1;
	imguiPoolInfo.poolSizeCount = static_cast<uint32_t>(imguiPoolSizes.size());
	imguiPoolInfo.pPoolSizes = imguiPoolSizes.data();

	if (vkCreateDescriptorPool(m_device->getLogical(), &imguiPoolInfo, nullptr, &m_imguiDescPool) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create ImGui descriptor pool");
		throw;
	}

	APP_LOG_INFO("Descriptor pool initialization successful");
}

DescriptorSet DescriptorPool::allocateDescriptorSet(DescriptorSetLayout& layout)
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool     = m_pool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts        = &layout.layout;

	VkDescriptorSet set;
	if (vkAllocateDescriptorSets(m_device->getLogical(), &allocInfo, &set) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to allocate descriptor set");
		throw;
	}

	return DescriptorSet(set, &layout);
}

VkDescriptorPool DescriptorPool::getImguiPool()
{
	return m_imguiDescPool;
}

void DescriptorPool::cleanup()
{
	APP_LOG_INFO("Destroying descriptor pool");

	vkDestroyDescriptorPool(m_device->getLogical(), m_pool, nullptr);
	vkDestroyDescriptorPool(m_device->getLogical(), m_imguiDescPool, nullptr);
}
