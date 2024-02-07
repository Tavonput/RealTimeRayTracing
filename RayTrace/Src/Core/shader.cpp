#include "pch.h"

#include "shader.h"

RasterShaderSet::RasterShaderSet(const char* vertPath, const char* fragPath, const Device& device)
{
	m_device = &device;

	APP_LOG_INFO("Loading shaders {} {}", vertPath, fragPath);

	// Read files
	m_vertCode = readFile(vertPath);
	m_fragCode = readFile(fragPath);

	// Create modules
	m_vertModule = createShaderModule(m_vertCode);
	m_fragModule = createShaderModule(m_fragCode);

	// Vertex shader stage
	m_shaderStages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_shaderStages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
	m_shaderStages[0].module = m_vertModule;
	m_shaderStages[0].pName  = "main";

	// Fragment shader stage
	m_shaderStages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_shaderStages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
	m_shaderStages[1].module = m_fragModule;
	m_shaderStages[1].pName  = "main";
}

VkPipelineShaderStageCreateInfo* RasterShaderSet::getStages()
{
	return m_shaderStages.data();
}

void RasterShaderSet::cleanup()
{
	vkDestroyShaderModule(m_device->getLogical(), m_vertModule, nullptr);
	vkDestroyShaderModule(m_device->getLogical(), m_fragModule, nullptr);
}

VkShaderModule RasterShaderSet::createShaderModule(const std::vector<char>& code)
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

std::vector<char> RasterShaderSet::readFile(const std::string& filename)
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
