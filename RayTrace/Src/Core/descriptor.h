#pragma once

#include "Application/logging.h"

#include "device.h"
#include "buffer.h"

/*****************************************************************************************************************
 * Binding Enums
 * 
 * These are just enums that specify the corresponding descriptor set bindings used the in the shaders.
 * 
 */
enum class SceneBinding
{
	GLOBAL   = 0,
	OBJ_DESC = 1
};

enum class RtxBinding
{
	TLAS      = 0,
	OUT_IMAGE = 1
};

/*****************************************************************************************************************
 * @class Descriptor Set Layout
 *
 * Wrapper around the Vulkan descriptor set layout.
 *
 * Provides a builder that is used to create layouts. Think of the builder as providing a blank layout that you
 * can add bindings to construct your desired layout. The builder should be reset between builds.
 *
 * Example Usage:
 *     auto builder = DescriptorSetLayout::Builder(device);
 *     
 *     builder.addBinding(
 *         0,
 *         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
 *         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
 *     
 *     DescriptorSetLayout layout = builder.buildLayout("name");
 *
 */
class DescriptorSetLayout
{
public:
	// Builder class
	class Builder
	{
	public:
		Builder(const Device& device)
			: m_device(&device) {}

		DescriptorSetLayout buildLayout(const std::string name);
		void reset();

		void addBinding(
			uint32_t           binding,
			VkDescriptorType   descriptorType,
			uint32_t           descriptorCount,
			VkShaderStageFlags stageFlags,
			VkSampler*         pImmutableSamplers = nullptr);

	private:
		const Device* m_device = nullptr;

		std::vector<VkDescriptorSetLayoutBinding> m_bindings;
	};

	// Descriptor Set Layout class
	VkDescriptorSetLayout                     layout = VK_NULL_HANDLE;
	std::vector<VkDescriptorSetLayoutBinding> bindings{};

	uint32_t numBindings = 0;

	DescriptorSetLayout() = default;

	DescriptorSetLayout(VkDescriptorSetLayout _layout, std::vector<VkDescriptorSetLayoutBinding> _bindings, uint32_t _numBindings, const std::string name)
		: layout(_layout), bindings(_bindings), numBindings(_numBindings), m_name(name) {}

	void cleanup(const Device& device);

private:
	std::string m_name = "";
};

/*****************************************************************************************************************
 * @class Descriptor Set
 *
 * Wrapper around the Vulkan descriptor set.
 *
 * Descriptor sets are meant to be allocated and provided by a descriptor pool.
 *
 * Descriptor sets are updated individually. Set the number of writes you want, add the writes that you want, then 
 * call update().
 *
 * Example Usage:
 *     DescriptorSet set = descriptorPool.allocateDescriptorSet(descriptorSetLayout);
 *     set.setTotalWriteCounts(1, 0, 0);
 *     set.addBufferWrite(buffer, BufferType::UNIFORM, 0, 0);
 *     set.update(device);
 *
 */
class DescriptorSet
{
public:
	DescriptorSet() = default;

	DescriptorSet(VkDescriptorSet set, DescriptorSetLayout* layout)
		: m_set(set), m_layout(layout) {}

	void setTotalWriteCounts(uint32_t bufferCount, uint32_t imageCount, uint32_t accelCount);

	/**
	 * Add a buffer write.
	 *
	 * @param buffer: The buffer that will be used for the write.
	 * @param type: The buffer type.
	 * @param offset: The offset.
	 * @param binding: The binding of the buffer.
	 */
	void addBufferWrite(Buffer buffer, BufferType type, VkDeviceSize offset, uint32_t binding);

	/*
	 * Add an image write.
	 * 
	 * @param imageInfo: A VkDescriptorImageInfo that specifies the sampler, view, and layout.
	 * @param binding: The binding of the image.
	 * @param storage: Is the image a storage image (default false).
	 */
	void addImageWrite(VkDescriptorImageInfo imageInfo, uint32_t binding, bool storage = false);
	 
	void addAccelerationStructureWrite(const VkAccelerationStructureKHR& accelerationStructure, uint32_t count, uint32_t binding);

	void update(const Device& device);

	VkDescriptorSet& getSet();

private:
	VkDescriptorSet      m_set    = VK_NULL_HANDLE;
	DescriptorSetLayout* m_layout = nullptr;

	std::vector<VkWriteDescriptorSet>                         m_descriptorWrites{};
	std::vector<VkWriteDescriptorSetAccelerationStructureKHR> m_accelWrites{};

	std::vector<VkDescriptorBufferInfo> m_writeBufferInfos{};
	std::vector<VkDescriptorImageInfo>  m_writeImageInfos{};

	uint32_t m_accelCount  = 0;
	uint32_t m_bufferCount = 0;
	uint32_t m_imageCount  = 0;
};

/*****************************************************************************************************************
 * @class Descriptor Pool
 * 
 * Wrapper around the Vulkan descriptor pool. 
 * 
 * Fill out a create info and set the descriptor count fields of the types that you want. Then allocate descriptor 
 * sets off specific layouts.
 * 
 * The creator of the descriptor set is responsible for calling its cleanup().
 * 
 * Example Usage:
 * 
 *     DescriptorPool::CreatInfo info{};
 *     info.pDevice = device;
 *     info.maxSets = 1;
 *     info.name    = "hehehe"
 *     
 *     info.poolSize           = 2;
 *     info.uniformBufferCount = 1;
 *     info.storageImageCount  = 1;
 *     
 *     DescriptorPool pool;
 *     pool.init(info);
 *     
 *     DescriptorSet set = allocateDescriptorSet(layout);
 * 
 */
class DescriptorPool
{
public:
	struct CreateInfo
	{
		const Device* pDevice = nullptr;

		VkDescriptorPoolCreateFlags flags    = 0;
		uint32_t                    maxSets  = 0;
		uint32_t                    poolSize = 0;
		const char*                 name     = "";

		uint32_t uniformBufferCount         = 0;
		uint32_t storageBufferCount         = 0;
		uint32_t combinedImageSamplerCount  = 0;
		uint32_t storageImageCount          = 0;
		uint32_t accelerationStructureCount = 0;
	};

	DescriptorPool() = default;

	/**
	 * Initialize a descriptor pool. When making the create info, only specify counts for the descriptors that you want 
	 * to use.
	 * 
	 * @param info: The create info
	 */
	void init(DescriptorPool::CreateInfo& info);

	/**
	 * Allocate a descriptor set given a descriptor set layout.
	 * 
	 * @param layout: Descriptor set layout
	 * 
	 * @return An allocated descriptor set
	 */
	DescriptorSet allocateDescriptorSet(DescriptorSetLayout& layout);

	VkDescriptorPool getPool() { return m_pool; }

	void cleanup();

private:
	const Device* m_device = nullptr;
	std::string   m_name = "";

	VkDescriptorPool m_pool = VK_NULL_HANDLE;
};
