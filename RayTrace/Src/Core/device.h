#pragma once

#include "Application/logging.h"

#include "extensions.h"

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	std::optional<uint32_t> computeFamily;

	bool isComplete() const
	{
		return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
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
		bool enableRaytracing);

	const VkPhysicalDevice& getPhysical() const { return m_physical; }
	const VkDevice& getLogical() const { return m_logical; }
	const QueueFamilyIndices& getIndices() const { return m_indices; }
	const VkQueue& getGraphicsQueue() const { return m_graphicsQueue; }
	const VkQueue& getPresentQueue() const { return m_presentQueue; }
	const VkQueue& getComputeQueue() const { return m_computeQueue; }
	const VkPhysicalDeviceRayTracingPipelinePropertiesKHR& getRtxProperties() const { return m_rtxProperties; }

	const void waitForGPU() const { vkDeviceWaitIdle(m_logical); }
	bool isRtxSupported() const { return m_enabledRaytracing; }

	VkFormat findSupportedFormat(
		const std::vector<VkFormat>& candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features) const;

	VkSampleCountFlagBits getMaxUsableSampleCount() const;

	static SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

	static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VkPhysicalDevice& physicalDevice);

	void cleanup();

private:
	VkPhysicalDevice m_physical = VK_NULL_HANDLE;
	VkDevice         m_logical  = VK_NULL_HANDLE;

	QueueFamilyIndices m_indices;
	VkQueue            m_graphicsQueue = VK_NULL_HANDLE;
	VkQueue            m_presentQueue  = VK_NULL_HANDLE;
	VkQueue            m_computeQueue  = VK_NULL_HANDLE;

	std::vector<const char*> m_instanceLayers;
	std::vector<const char*> m_deviceExtensions;

	VkPhysicalDeviceRayTracingPipelinePropertiesKHR m_rtxProperties{};
	bool                                            m_enabledRaytracing = false;

	void pickPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface);
	void createLogicalDevice();

	bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	void setDeviceFeatures(VkPhysicalDeviceFeatures2& deviceFeatures);
};