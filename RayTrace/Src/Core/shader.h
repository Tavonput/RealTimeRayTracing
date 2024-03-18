#pragma once

#include "Application/logging.h"
#include "device.h"
#include "buffer.h"

/*****************************************************************************************************************
 *
 * Enums for the different shader stages.
 *
 */
enum class ShaderStage
{
	NONE = 0,
	VERT,
	FRAG,
	RGEN,
	MISS,
	CHIT,
	ENUM_MAX
};

/*****************************************************************************************************************
 *
 * @class Shader Set
 *
 *
 *
 */
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
	const std::array<uint32_t, (size_t)ShaderStage::ENUM_MAX>& getStageCounts() const { return m_stageCount; }

	void cleanup();

private:
	const Device* m_device = nullptr;

	std::vector<VkPipelineShaderStageCreateInfo>      m_shaderStages;
	std::vector<VkShaderModule>                       m_modules;
	std::vector<VkRayTracingShaderGroupCreateInfoKHR> m_shaderGroup;

	std::array<uint32_t, (size_t)ShaderStage::ENUM_MAX> m_stageCount{ 0 };

	VkShaderModule createShaderModule(const std::vector<char>& code);
	std::vector<char> readFile(const std::string& filename);
};

/*****************************************************************************************************************
 *
 * @class Shader Binding Table
 *
 * 
 *
 */
class ShaderBindingTable
{
public:
	enum SbtRegion
	{
		RGEN = 0,
		MISS,
		HIT,
		CALL
	};

	void build(const Device& device, VkPipeline& rtxPipeline, ShaderSet& shaders, const std::string& name);

	void cleanup();

	std::array<VkStridedDeviceAddressRegionKHR, 4>& getRegions() { return m_regions; }

private:
	const Device* m_device = nullptr;
	std::string   m_name   = "";

	Buffer m_sbtBuffer;

	std::array<VkStridedDeviceAddressRegionKHR, 4> m_regions;

	uint32_t alignUp(uint32_t x, uint32_t y);
};