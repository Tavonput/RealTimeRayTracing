#include "pch.h"

#include "device.h"

void Device::init(VkInstance& instance, VkSurfaceKHR& surface, std::vector<const char*> instanceLayers, bool enableRaytracing)
{
	m_instanceLayers = instanceLayers;
	m_enabledRaytracing = enableRaytracing;

	pickPhysicalDevice(instance, surface);
	createLogicalDevice();

	if (m_enabledRaytracing)
		loadDeviceExtensionsRayTrace(m_logical);
}

VkFormat Device::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_physical, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			return format;

		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			return format;
	}

	APP_LOG_CRITICAL("Failed to find supported format");
	throw;
}

VkSampleCountFlagBits Device::getMaxUsableSampleCount() const
{
	// Find max samples for MSAA
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(m_physical, &physicalDeviceProperties);

	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
	if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT)  { return VK_SAMPLE_COUNT_8_BIT; }
	if (counts & VK_SAMPLE_COUNT_4_BIT)  { return VK_SAMPLE_COUNT_4_BIT; }
	if (counts & VK_SAMPLE_COUNT_2_BIT)  { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

SwapChainSupportDetails Device::querySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
{
	SwapChainSupportDetails details;

	// Query surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	// Query supported surface formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	// Query supported presentation modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VkPhysicalDevice& physicalDevice)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;

	APP_LOG_CRITICAL("Failed to find suitable memory type");
	throw;
}

void Device::cleanup()
{
	APP_LOG_INFO("Destroying devices");

	vkDestroyDevice(m_logical, nullptr);
}

void Device::pickPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface)
{
	APP_LOG_INFO("Choosing physical device");

	// Extensions
	m_deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	if (m_enabledRaytracing)
	{
		m_deviceExtensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
		m_deviceExtensions.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
		m_deviceExtensions.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
	}

	// Find GPUs
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		APP_LOG_CRITICAL("Failed to find any GPUs with Vulkan support");
		throw;
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	// Check for a GPU that has supported features
	for (const auto& device : devices)
	{
		if (Device::isDeviceSuitable(device, surface))
		{
			m_physical = device;
			break;
		}
	}

	if (m_physical == VK_NULL_HANDLE)
	{
		APP_LOG_CRITICAL("Failed to find a suitable GPU");
		throw;
	}

	APP_LOG_INFO("Physical device was found");

	// Set indices
	m_indices = findQueueFamilies(m_physical, surface);

	// Query for raytracing properties
	if (m_enabledRaytracing)
	{
		m_rtxProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

		VkPhysicalDeviceProperties2 properties{};
		properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		properties.pNext = &m_rtxProperties;
		vkGetPhysicalDeviceProperties2(m_physical, &properties);
	}
}

void Device::createLogicalDevice()
{
	APP_LOG_INFO("Initializing logical device");

	// Queue family create infos
	std::set<uint32_t> uniqueQueueFamilies = {
		m_indices.graphicsFamily.value(),
		m_indices.presentFamily.value()
	};

	float queuePriority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount       = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Buffer addresses
	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceFeatures{};
	bufferDeviceFeatures.sType               = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	bufferDeviceFeatures.bufferDeviceAddress = VK_TRUE;

	// Ray Tracing
	VkPhysicalDeviceAccelerationStructureFeaturesKHR accelFeatures{};
	accelFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	accelFeatures.pNext = &bufferDeviceFeatures;
	if (m_enabledRaytracing)
		accelFeatures.accelerationStructure = VK_TRUE;


	// Device features
	VkPhysicalDeviceFeatures2 deviceFeatures{};
	deviceFeatures.sType                      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceFeatures.pNext                      = &accelFeatures;
	setDeviceFeatures(deviceFeatures);

	// Device create
	VkDeviceCreateInfo createInfo{};
	createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos       = queueCreateInfos.data();
	createInfo.enabledExtensionCount   = static_cast<uint32_t>(m_deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();
	createInfo.enabledLayerCount       = static_cast<uint32_t>(m_instanceLayers.size());
	createInfo.ppEnabledLayerNames     = m_instanceLayers.data();
	createInfo.pNext                   = &deviceFeatures;

	if (vkCreateDevice(m_physical, &createInfo, nullptr, &m_logical) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create logical device");
		throw;
	}

	// Store queues
	vkGetDeviceQueue(m_logical, m_indices.graphicsFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_logical, m_indices.presentFamily.value(), 0, &m_presentQueue);

	APP_LOG_INFO("Logical device initialization successful");
}

bool Device::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	// Check queue families
	QueueFamilyIndices indices = findQueueFamilies(device, surface);

	// Check device extensions
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	// Check swapchain support
	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	// Anisotropy support
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;
	
	// Enumerate queue families
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	// Find queue families for graphics and present
	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport)
			indices.presentFamily = i;

		if (indices.isComplete())
			break;
		i++;
	}

	return indices;
}

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	// Enumerate supported extensions
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	// Ensure the required extensions are supported
	std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());
	for (const auto& extension : availableExtensions)
		requiredExtensions.erase(extension.extensionName);

	return requiredExtensions.empty();
}

void Device::setDeviceFeatures(VkPhysicalDeviceFeatures2& deviceFeatures)
{
	VkPhysicalDeviceFeatures supportedFeatures{};
	vkGetPhysicalDeviceFeatures(m_physical, &supportedFeatures);

	// Anisotropic filtering
	if (supportedFeatures.samplerAnisotropy)
		deviceFeatures.features.samplerAnisotropy = VK_TRUE;
	else
		APP_LOG_WARN("Requested device with anisotropic filtering but it is not supported. MAY CAUSE ERRORS");

	// Sample rate shading
	if (supportedFeatures.sampleRateShading)
		deviceFeatures.features.sampleRateShading = VK_TRUE;
	else
		APP_LOG_WARN("Requested device with sample shading but it is not supported. MAY CAUSE ERRORS");

	// Shader 64-bit integers
	if (supportedFeatures.shaderInt64)
		deviceFeatures.features.shaderInt64 = VK_TRUE;
	else
		APP_LOG_WARN("Requested device with shader 64-bit integers but it is not supported. MAY CAUSE ERRORS");

	// Geometry shaders
	if (supportedFeatures.geometryShader)
		deviceFeatures.features.geometryShader = VK_TRUE;
	else
		APP_LOG_WARN("Requested device with geometry shaders but it is not supported. MAY CAUSE ERRORS");
}
