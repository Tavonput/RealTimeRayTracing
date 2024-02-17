#include "pch.h"
#include "descriptor.h"

// -----------------------------------------------------
// -----------------------------------------------------
// Descriptor Set Layout
// -----------------------------------------------------
// -----------------------------------------------------
DescriptorSetLayout DescriptorSetLayout::Builder::buildLayout()
{
	APP_LOG_INFO("Building descriptor set layout");

	VkDescriptorSetLayoutCreateInfo info{};
	info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.bindingCount = static_cast<uint32_t>(m_bindings.size());
	info.pBindings    = m_bindings.data();

	VkDescriptorSetLayout layout;
	if (vkCreateDescriptorSetLayout(m_device->getLogical(), &info, nullptr, &layout) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create descriptor set layout");
		throw;
	};

	return DescriptorSetLayout(layout, m_bindings, static_cast<uint32_t>(m_bindings.size()));
}

void DescriptorSetLayout::Builder::addBinding(
	SceneBinding       binding, 
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
	APP_LOG_INFO("Destroying descriptor set layout");

	vkDestroyDescriptorSetLayout(device.getLogical(), layout, nullptr);
}

// -----------------------------------------------------
// -----------------------------------------------------
// Descriptor Set
// -----------------------------------------------------
// -----------------------------------------------------
void DescriptorSet::addBufferWrite(Buffer buffer, VkDeviceSize offset, SceneBinding binding)
{
	// Write info
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = buffer.getBuffer();
	bufferInfo.offset = offset;
	bufferInfo.range  = VK_WHOLE_SIZE;

	m_writeBufferInfos.push_back(bufferInfo);

	// Descriptor write
	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet          = m_set;
	descriptorWrite.dstBinding      = static_cast<uint32_t>(binding);
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo     = &m_writeBufferInfos[m_currentWrite];

	m_descriptorWrites.push_back(descriptorWrite);
}

void DescriptorSet::update(const Device& device)
{
	vkUpdateDescriptorSets(device.getLogical(), static_cast<uint32_t>(m_descriptorWrites.size()), m_descriptorWrites.data(), 0, nullptr);
}

VkDescriptorSet& DescriptorSet::getSet()
{
	return m_set;
}

// -----------------------------------------------------
// -----------------------------------------------------
// Descriptor Pool
// -----------------------------------------------------
// -----------------------------------------------------
void DescriptorPool::init(const Device& device, uint32_t framesInFlight, uint32_t setsPerFrame)
{
	APP_LOG_INFO("Initializing descriptor pool");

	m_device = &device;

	// Pool sizes
	// TODO: Figure out the correct pool sizes depending on the layouts
	//std::array<VkDescriptorPoolSize, 1> poolSizes{};
	std::array<VkDescriptorPoolSize, 2> poolSizes{};  //***Used to be 1
	poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = framesInFlight;

	//***Changes
	poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = framesInFlight * 2;

	// Descriptor pool creation
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; //***Change
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes    = poolSizes.data();
	poolInfo.maxSets       = setsPerFrame * framesInFlight;

	if (vkCreateDescriptorPool(m_device->getLogical(), &poolInfo, nullptr, &m_pool) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create descriptor pool");
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

VkDescriptorPool DescriptorPool::getPool() {
	return m_pool;
}

void DescriptorPool::cleanup()
{
	APP_LOG_INFO("Destroying descriptor pool");

	vkDestroyDescriptorPool(m_device->getLogical(), m_pool, nullptr);
}
