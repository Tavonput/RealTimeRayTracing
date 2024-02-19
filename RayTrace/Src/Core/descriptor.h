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

		DescriptorSetLayout buildLayout();

		void addBinding(
			SceneBinding       binding,
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

	DescriptorSetLayout() {}

	DescriptorSetLayout(VkDescriptorSetLayout _layout, std::vector<VkDescriptorSetLayoutBinding> _bindings, uint32_t _numBindings)
		: layout(_layout), bindings(_bindings), numBindings(_numBindings) {}

	void cleanup(const Device& device);
};

class DescriptorSet
{
public:
	DescriptorSet(VkDescriptorSet set, DescriptorSetLayout* layout)
		: m_set(set), m_layout(layout) {}

	void addBufferWrite(Buffer buffer, BufferType type, VkDeviceSize offset, SceneBinding binding);
	void update(const Device& device);

	VkDescriptorSet& getSet();

private:
	VkDescriptorSet      m_set = VK_NULL_HANDLE;
	DescriptorSetLayout* m_layout = nullptr;

	std::vector<VkWriteDescriptorSet>   m_descriptorWrites{};
	std::vector<VkDescriptorBufferInfo> m_writeBufferInfos{};
};

class DescriptorPool
{
public:
	void init(const Device& device, uint32_t framesInFlight, uint32_t setsPerFrame);

	DescriptorSet allocateDescriptorSet(DescriptorSetLayout& layout);

	VkDescriptorPool getPool(); //Returns the VkDescriptorPool. Used by ImGui

	void cleanup();

private:
	const Device* m_device = nullptr;

	VkDescriptorPool m_pool = VK_NULL_HANDLE;
};
