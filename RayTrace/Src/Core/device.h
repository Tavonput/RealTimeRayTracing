#pragma once

#include <vulkan/vulkan.h>

#include <optional>
#include <set>
#include <vector>

#include "Application/logging.h"

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() const
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR        capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR>   presentModes;
};

class Device
{
public:
	void init(
		VkInstance& instance,
		VkSurfaceKHR& surface,
		std::vector<const char*> instanceLayers,
		Logger logger);

	const VkPhysicalDevice& getPhysical() const;
	const VkDevice& getLogical() const;
	const QueueFamilyIndices& getIndicies() const;
	const VkQueue& getGraphicsQueue() const;

	VkFormat findSupportedFormat(
		const std::vector<VkFormat>& candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features);

	VkSampleCountFlagBits getMaxUsableSampleCount();

	static SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

	static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VkPhysicalDevice& physicalDevice);

	void cleanup();

private:
	VkPhysicalDevice m_physical = VK_NULL_HANDLE;
	VkDevice         m_logical  = VK_NULL_HANDLE;

	QueueFamilyIndices m_indices;
	VkQueue            m_graphicsQueue = VK_NULL_HANDLE;
	VkQueue            m_presentQueue  = VK_NULL_HANDLE;

	std::vector<const char*> m_instanceLayers;
	std::vector<const char*> m_deviceExtensions;

	Logger m_logger;

	void pickPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface);
	void createLogicalDevice();

	bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
};