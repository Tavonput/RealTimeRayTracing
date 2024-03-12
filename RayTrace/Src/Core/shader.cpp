#include "pch.h"

#include "shader.h"

void ShaderSet::addShader(ShaderStage type, const char* filepath)
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
		case ShaderStage::VERT: stage.stage = VK_SHADER_STAGE_VERTEX_BIT;          break;
		case ShaderStage::FRAG: stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;        break;
		case ShaderStage::RGEN: stage.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;      break;
		case ShaderStage::MISS: stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;        break;
		case ShaderStage::CHIT: stage.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR; break;
	}

	m_shaderStages.emplace_back(stage);
}

void ShaderSet::setupRtxShaderGroup()
{
	for (uint32_t i = 0; i < m_shaderStages.size(); i++)
	{
		VkRayTracingShaderGroupCreateInfoKHR group{};
		group.sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		group.generalShader      = VK_SHADER_UNUSED_KHR;
		group.closestHitShader   = VK_SHADER_UNUSED_KHR;
		group.anyHitShader       = VK_SHADER_UNUSED_KHR;
		group.intersectionShader = VK_SHADER_UNUSED_KHR;

		switch (m_shaderStages[i].stage)
		{
			case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
				group.type          = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
				group.generalShader = i;
				m_shaderGroup.emplace_back(group);
				break;

			case VK_SHADER_STAGE_MISS_BIT_KHR:
				group.type          = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
				group.generalShader = i;
				m_shaderGroup.emplace_back(group);
				break;

			case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
				group.type             = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
				group.closestHitShader = i;
				m_shaderGroup.emplace_back(group);
				break;
		}
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
