#pragma once

#include "image.h"
#include "device.h"

#include "Application/logging.h"

class DepthBuffer
{
public:
	Image    image;
	VkFormat format = VK_FORMAT_UNDEFINED;

	DepthBuffer() = default;

	DepthBuffer(
		const Device& device,
		VkExtent2D extent,
		VkSampleCountFlagBits numSamples,
		const std::string name);

	void cleanup();

private:
	const Device* m_device = nullptr;
	std::string   m_name = "";
};