#pragma once

#include "Application/logging.h"
#include "device.h"

class RasterShaderSet
{
public:
	RasterShaderSet(const char* vertPath, const char* fragPath, const Device& device);

	VkPipelineShaderStageCreateInfo* getStages();

	void cleanup();

private:
	const Device* m_device = nullptr;

	std::array<VkPipelineShaderStageCreateInfo, 2> m_shaderStages{};

	std::vector<char> m_vertCode;
	std::vector<char> m_fragCode;

	VkShaderModule m_vertModule = VK_NULL_HANDLE;
	VkShaderModule m_fragModule = VK_NULL_HANDLE;

	VkShaderModule createShaderModule(const std::vector<char>& code);
	std::vector<char> readFile(const std::string& filename);
};
