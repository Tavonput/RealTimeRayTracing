#pragma once

#include <vulkan/vulkan.h>

#include <fstream>

#include "Application/logging.h"
#include "device.h"

class RasterShaderSet
{
public:
	RasterShaderSet(const char* vertPath, const char* fragPath, const Device& device, Logger logger);

	VkPipelineShaderStageCreateInfo* getStages();

	void cleanup();

private:
	const Device* m_device = nullptr;

	Logger m_logger;

	std::array<VkPipelineShaderStageCreateInfo, 2> m_shaderStages{};

	std::vector<char> m_vertCode;
	std::vector<char> m_fragCode;

	VkShaderModule m_vertModule = VK_NULL_HANDLE;
	VkShaderModule m_fragModule = VK_NULL_HANDLE;

	VkShaderModule createShaderModule(const std::vector<char>& code);
	std::vector<char> readFile(const std::string& filename);
};
