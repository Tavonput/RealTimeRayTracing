#pragma once

#include "Application/logging.h"

#include "device.h"
#include "buffer.h"

enum class SceneBinding
{
	GLOBAL = 0,
	MATERIAL = 1
};

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

class DescriptorSet
{
public:
	DescriptorSet(VkDescriptorSet set, DescriptorSetLayout* layout)
		: m_set(set), m_layout(layout) {}

	void addBufferWrite(Buffer buffer, BufferType type, VkDeviceSize offset, uint32_t binding);
	void addImageWrite(VkImageView imageView, VkSampler sampler, uint32_t binding);
	void update(const Device& device);

	VkDescriptorSet& getSet();

private:
	VkDescriptorSet      m_set    = VK_NULL_HANDLE;
	DescriptorSetLayout* m_layout = nullptr;

	std::vector<VkWriteDescriptorSet>   m_descriptorWrites{};
	std::vector<VkDescriptorBufferInfo> m_writeBufferInfos{};
	std::vector<VkDescriptorImageInfo>  m_writeImageInfos{};
};

class DescriptorPool
{
public:
	void init(const Device& device, uint32_t framesInFlight, uint32_t setsPerFrame);

	DescriptorSet allocateDescriptorSet(DescriptorSetLayout& layout);

	VkDescriptorPool getImguiPool(); //Returns the VkDescriptorPool. Used by ImGui

	void cleanup();

private:
	const Device* m_device = nullptr;

	VkDescriptorPool m_pool = VK_NULL_HANDLE;
	VkDescriptorPool m_imguiDescPool = VK_NULL_HANDLE;
};
