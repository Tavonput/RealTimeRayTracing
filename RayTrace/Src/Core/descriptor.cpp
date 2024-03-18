#include "pch.h"
#include "descriptor.h"

/*****************************************************************************************************************
 *
 * Descriptor Set Layout
 *
 */
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
		throw std::exception();
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

void DescriptorSet::setTotalWriteCounts(uint32_t bufferCount, uint32_t imageCount, uint32_t accelCount)
{
	m_descriptorWrites.reserve(bufferCount + imageCount + accelCount);
	m_accelWrites.reserve(accelCount);
	m_writeBufferInfos.reserve(bufferCount);
	m_writeImageInfos.reserve(imageCount);
}

/*****************************************************************************************************************
 *
 * Descriptor Set
 *
 */
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
	descriptorWrite.pBufferInfo     = &m_writeBufferInfos[m_bufferCount];

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
		throw std::exception();
	}

	m_descriptorWrites.emplace_back(descriptorWrite);

	m_bufferCount++;
}

void DescriptorSet::addImageWrite(VkDescriptorImageInfo imageInfo, uint32_t binding, bool storage)
{
	m_writeImageInfos.emplace_back(imageInfo);

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet          = m_set;
	descriptorWrite.dstBinding      = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.pImageInfo      = &m_writeImageInfos[m_imageCount];

	if (storage)
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

	m_descriptorWrites.push_back(descriptorWrite);

	m_imageCount++;
}

void DescriptorSet::addAccelerationStructureWrite(const VkAccelerationStructureKHR& accelerationStructure, uint32_t count, uint32_t binding)
{
	VkWriteDescriptorSetAccelerationStructureKHR accelWrite{};
	accelWrite.sType                      = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
	accelWrite.accelerationStructureCount = count;
	accelWrite.pAccelerationStructures    = &accelerationStructure;

	m_accelWrites.emplace_back(accelWrite);

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet          = m_set;
	descriptorWrite.dstBinding      = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pNext           = &m_accelWrites[m_accelCount];

	m_descriptorWrites.emplace_back(descriptorWrite);

	m_accelCount++;
}

void DescriptorSet::update(const Device& device)
{
	// Update
	vkUpdateDescriptorSets(device.getLogical(), static_cast<uint32_t>(m_descriptorWrites.size()), m_descriptorWrites.data(), 0, nullptr);

	// Reset
	m_descriptorWrites = std::vector<VkWriteDescriptorSet>{};
	m_accelWrites      = std::vector<VkWriteDescriptorSetAccelerationStructureKHR>{};
	m_writeBufferInfos = std::vector<VkDescriptorBufferInfo>{};
	m_writeImageInfos  = std::vector<VkDescriptorImageInfo>{};

	m_bufferCount = 0;
	m_imageCount  = 0;
	m_accelCount  = 0;
}

VkDescriptorSet& DescriptorSet::getSet()
{
	return m_set;
}

/*****************************************************************************************************************
 * 
 * Descriptor Pool
 *
 */
void DescriptorPool::init(DescriptorPool::CreateInfo& info)
{
	APP_LOG_INFO("Initializing descriptor pool ({})", info.name);

	m_device = info.pDevice;
	m_name   = info.name;

	// Setup the pool sizes
	std::vector<VkDescriptorPoolSize> poolSizes(info.poolSize);
	uint32_t poolIndex = 0;

	// Lambda that helps with setting up the pool sizes based on the types specified in the create info
	auto setPoolSize = [&](VkDescriptorType type, uint32_t count)
	{
		if (count != 0)
		{
			poolSizes[poolIndex].type = type;
			poolSizes[poolIndex].descriptorCount = count;
			poolIndex++;
		}
	};

	setPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,             info.uniformBufferCount);
	setPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,             info.storageBufferCount);
	setPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,     info.combinedImageSamplerCount);
	setPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,              info.storageImageCount);
	setPoolSize(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, info.accelerationStructureCount);

	// Descriptor pool creation
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags         = info.flags;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes    = poolSizes.data();
	poolInfo.maxSets       = info.maxSets;

	if (vkCreateDescriptorPool(m_device->getLogical(), &poolInfo, nullptr, &m_pool) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create descriptor pool ({})", info.name);
		throw std::exception();
	}
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
		throw std::exception();
	}

	return DescriptorSet(set, &layout);
}

void DescriptorPool::cleanup()
{
	APP_LOG_INFO("Destroying descriptor pool ({})", m_name);

	vkDestroyDescriptorPool(m_device->getLogical(), m_pool, nullptr);
}
