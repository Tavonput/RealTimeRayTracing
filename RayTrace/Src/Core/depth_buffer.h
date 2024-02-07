#pragma once

#include "image.h"
#include "device.h"

#include "Application/logging.h"

class DepthBuffer
{
public:
	Image    image;
	VkFormat format = VK_FORMAT_UNDEFINED;

	DepthBuffer() {}

	DepthBuffer(
		const Device& device,
		VkExtent2D& extent,
		VkSampleCountFlagBits numSamples);

	void cleanup();

private:
	const Device* m_device = nullptr;
};