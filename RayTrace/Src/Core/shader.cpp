#include "pch.h"

#include "shader.h"

/*****************************************************************************************************************
 *
 * Shader Set
 *
 */
void ShaderSet::init(const Device& device, uint32_t numHitGroups)
{
	m_device = &device;
	m_hitGroups = std::vector<HitGroup>(numHitGroups);
}

void ShaderSet::addShader(ShaderStage type, const char* filepath, uint32_t hitGroup)
{
	APP_LOG_INFO("Loading shaders {}", filepath);

	std::vector<char> m_code = readFile(filepath);

	VkShaderModule module = createShaderModule(m_code);
	m_modules.emplace_back(module);

	VkPipelineShaderStageCreateInfo stage{};
	stage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stage.module = module;
	stage.pName  = "main";

	switch (type)
	{
		case ShaderStage::VERT: 
			stage.stage = VK_SHADER_STAGE_VERTEX_BIT;          
			m_stageCount[(size_t)ShaderStage::VERT]++;
			break;

		case ShaderStage::FRAG:
			stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			m_stageCount[(size_t)ShaderStage::FRAG]++;
			break;

		case ShaderStage::RGEN:
			stage.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
			m_stageCount[(size_t)ShaderStage::RGEN]++;
			break;

		case ShaderStage::MISS:
			stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
			m_stageCount[(size_t)ShaderStage::MISS]++;
			break;

		case ShaderStage::CHIT:
			stage.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			m_stageCount[(size_t)ShaderStage::CHIT]++;
			m_hitGroups[hitGroup].chitIndex = static_cast<uint32_t>(m_shaderStages.size());
			break;

		case ShaderStage::AHIT:
			stage.stage = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
			m_stageCount[(size_t)ShaderStage::AHIT]++;
			m_hitGroups[hitGroup].ahitIndex = static_cast<uint32_t>(m_shaderStages.size());
			break;
	}

	m_shaderStages.emplace_back(stage);
}

void ShaderSet::setupRtxShaderGroup()
{
	// Ray gen and miss groups
	for (uint32_t i = 0; i < m_shaderStages.size(); i++)
	{
		switch (m_shaderStages[i].stage)
		{
			case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
			case VK_SHADER_STAGE_MISS_BIT_KHR:
				VkRayTracingShaderGroupCreateInfoKHR group{};
				group.sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
				group.type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
				group.generalShader      = i;
				group.closestHitShader   = VK_SHADER_UNUSED_KHR;
				group.anyHitShader       = VK_SHADER_UNUSED_KHR;
				group.intersectionShader = VK_SHADER_UNUSED_KHR;

				m_shaderGroup.emplace_back(group);
				break;
		}
	}

	// Hit groups
	for (const auto& hitGroup : m_hitGroups)
	{
		VkRayTracingShaderGroupCreateInfoKHR group{};
		group.sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		group.type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
		group.generalShader      = VK_SHADER_UNUSED_KHR;
		group.closestHitShader   = hitGroup.chitIndex;
		group.anyHitShader       = hitGroup.ahitIndex;
		group.intersectionShader = VK_SHADER_UNUSED_KHR;

		m_shaderGroup.emplace_back(group);
	}
}

void ShaderSet::cleanup()
{
	for (auto& module : m_modules)
		vkDestroyShaderModule(m_device->getLogical(), module, nullptr);
}

VkShaderModule ShaderSet::createShaderModule(const std::vector<char>& code)
{
	// Create shader module
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode    = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_device->getLogical(), &createInfo, nullptr, &shaderModule))
	{
		APP_LOG_CRITICAL("Failed to create shader module");
		throw;
	}

	return shaderModule;
}

std::vector<char> ShaderSet::readFile(const std::string& filename)
{
	// Open binary file
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		APP_LOG_CRITICAL("Failed to open file {}", filename);
		throw;
	}

	// Read bytes from file
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

/*****************************************************************************************************************
 *
 * Shader Binding Table
 *
 */
void ShaderBindingTable::build(const Device& device, VkPipeline& rtxPipeline, ShaderSet& shaders, const std::string& name)
{
	APP_LOG_INFO("Building shader binding table ({})", name);

	m_device = &device;
	m_name   = name;
	VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtxProps = m_device->getRtxProperties();

	auto& shaderCounts = shaders.getStageCounts();
	uint32_t missCount   = shaderCounts[(size_t)ShaderStage::MISS];
	uint32_t hitCount    = shaders.getHitGroupCount();
	uint32_t handleCount = shaderCounts[(size_t)ShaderStage::RGEN] + missCount + hitCount;

	uint32_t handleSize        = rtxProps.shaderGroupHandleSize;
	uint32_t handleSizeAligned = alignUp(handleSize, rtxProps.shaderGroupHandleAlignment);

	m_regions[RGEN].stride = alignUp(handleSizeAligned, rtxProps.shaderGroupBaseAlignment);
	m_regions[RGEN].size   = m_regions[RGEN].stride;
	m_regions[MISS].stride = handleSizeAligned;
	m_regions[MISS].size   = alignUp(missCount * handleSizeAligned, rtxProps.shaderGroupBaseAlignment);
	m_regions[HIT].stride  = handleSizeAligned;
	m_regions[HIT].size    = alignUp(hitCount * handleSizeAligned, rtxProps.shaderGroupBaseAlignment);
	m_regions[CALL].stride = 0;
	m_regions[CALL].size   = 0;

	uint32_t dataSize = handleCount * handleSize;
	std::vector<uint8_t> handles(dataSize);
	if (vkGetRayTracingShaderGroupHandlesKHR(m_device->getLogical(), rtxPipeline, 0, handleCount, dataSize, handles.data()) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create shader group handles");
		throw std::exception();
	}

	Buffer::CreateInfo bufferInfo{};
	bufferInfo.device   = m_device;
	bufferInfo.dataSize = m_regions[RGEN].size + m_regions[MISS].size + m_regions[HIT].size + m_regions[CALL].size;
	bufferInfo.name     = "SBT Buffer";
	m_sbtBuffer = Buffer::CreateShaderBindingTableBuffer(bufferInfo);

	VkDeviceAddress sbtAddress    = m_sbtBuffer.getDeviceAddress();
	m_regions[RGEN].deviceAddress = sbtAddress;
	m_regions[MISS].deviceAddress = sbtAddress + m_regions[RGEN].size;
	m_regions[HIT].deviceAddress  = sbtAddress + m_regions[RGEN].size + m_regions[MISS].size;
	m_regions[CALL].deviceAddress = sbtAddress + m_regions[RGEN].size + m_regions[MISS].size + m_regions[HIT].size;

	auto getHandle = [&](int i)
	{
		return handles.data() + i * handleSize;
	};

	m_sbtBuffer.map();
	uint8_t* pSbtBuffer = reinterpret_cast<uint8_t*>(m_sbtBuffer.getMap());
	uint8_t* pData      = nullptr;
	uint32_t handleIdx  = 0;

	// Ray gen
	pData = pSbtBuffer;
	memcpy(pData, getHandle(handleIdx++), handleSize);

	// Miss
	pData = pSbtBuffer + m_regions[RGEN].size;
	for (uint32_t i = 0; i < missCount; i++)
	{
		memcpy(pData, getHandle(handleIdx++), handleSize);
		pData += m_regions[MISS].stride;
	}

	// Hit
	pData = pSbtBuffer + m_regions[RGEN].size + m_regions[MISS].size;
	for (uint32_t i = 0; i < hitCount; i++)
	{
		memcpy(pData, getHandle(handleIdx++), handleSize);
		pData += m_regions[HIT].stride;
	}

	m_sbtBuffer.unmap();
}

void ShaderBindingTable::cleanup()
{
	APP_LOG_INFO("Destroying shader binding table ({})", m_name);

	m_sbtBuffer.cleanup();
}

uint32_t ShaderBindingTable::alignUp(uint32_t size, uint32_t alignment)
{
	// https://nvpro-samples.github.io/vk_raytracing_tutorial_KHR/#shaderbindingtable
	return (size + (alignment - 1)) & ~(alignment - 1);
}
