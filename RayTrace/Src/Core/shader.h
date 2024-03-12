#pragma once

#include "Application/logging.h"
#include "device.h"


enum class ShaderStage
{
	NONE = 0,
	VERT,
	FRAG,
	RGEN,
	MISS,
	CHIT
};

class ShaderSet
{
public:
	ShaderSet() = default;
	ShaderSet(const Device& device) { init(device); }
	void init(const Device& device) { m_device = &device; }

	void addShader(ShaderStage type, const char* filepath);

	void setupRtxShaderGroup();

	VkPipelineShaderStageCreateInfo* getStages() { return m_shaderStages.data(); }
	VkRayTracingShaderGroupCreateInfoKHR* getShaderGroup() { return m_shaderGroup.data(); }
	uint32_t getStageCount() const { return static_cast<uint32_t>(m_shaderStages.size()); }
	uint32_t getGroupCount() const { return static_cast<uint32_t>(m_shaderGroup.size()); }

	void cleanup();

private:
	const Device* m_device = nullptr;

	std::vector<VkPipelineShaderStageCreateInfo>      m_shaderStages;
	std::vector<VkShaderModule>                       m_modules;
	std::vector<VkRayTracingShaderGroupCreateInfoKHR> m_shaderGroup;

	VkShaderModule createShaderModule(const std::vector<char>& code);
	std::vector<char> readFile(const std::string& filename);
};
