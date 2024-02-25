#include "pch.h"

#include "system_context.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT || messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		VAL_LOG_TRACE("{}", pCallbackData->pMessage);

	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		VAL_LOG_WARN("{}", pCallbackData->pMessage);

	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		VAL_LOG_ERROR("{}", pCallbackData->pMessage);

	return VK_FALSE;
}

void SystemContext::init(Window& window)
{
	initInstance();

#ifndef RT_DIST
	initDebugMessenger();
#endif

	initSurface(window);

	m_device.init(m_instance, m_surface, m_instanceLayers);
}

const Device& SystemContext::getDevice() const
{
	return m_device;
}

const VkSurfaceKHR& SystemContext::getSurface() const
{
	return m_surface;
}

VkInstance SystemContext::getInstance()
{
	return m_instance;
}

void SystemContext::cleanup()
{
	m_device.cleanup();

	APP_LOG_INFO("Destroying system context");

#ifndef RT_DIST
	destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
#endif

	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyInstance(m_instance, nullptr);
}

void SystemContext::initInstance()
{
	APP_LOG_INFO("Initializing Vulkan instance");

	std::vector<const char*> wantedLayers{};

	// Validation layers
#ifndef RT_DIST
	wantedLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	// Automatic FPS counter in window bar
	wantedLayers.push_back("VK_LAYER_LUNARG_monitor");

	// Check if instance layers are supported
	m_instanceLayers = checkLayerSupport(wantedLayers);

	// Application info
	VkApplicationInfo appInfo{};
	appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName   = "Real Time Raytracing";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName        = "No Engine";
	appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion         = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Extensions and layers
	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount       = static_cast<uint32_t>(m_instanceLayers.size());
	createInfo.ppEnabledLayerNames     = m_instanceLayers.data();

	// Debug messenger
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
#ifndef RT_DIST
	populateDebugMessengerCreateInfo(debugCreateInfo);
	createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#else
	createInfo.pNext = nullptr;
#endif

	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create Vulkan instance");
		throw;
	}

	APP_LOG_INFO("Vulkan instance initialization successful");
}

void SystemContext::initDebugMessenger()
{
	APP_LOG_INFO("Initializing debug messenger");

	// Create debug messenger
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (createDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
	{
		APP_LOG_CRITICAL("Failed to create debug messenger");
		throw;
	}

	APP_LOG_INFO("Debug messenger initialization successful");
}

void SystemContext::initSurface(Window& window)
{
	if (glfwCreateWindowSurface(m_instance, window.getWindowGLFW(), nullptr, &m_surface))
	{
		APP_LOG_CRITICAL("Failed to create window surface");
		throw;
	}
}

VkResult SystemContext::createDebugUtilsMessengerEXT(
	VkInstance instance, 
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
	const VkAllocationCallbacks* pAllocator, 
	VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void SystemContext::destroyDebugUtilsMessengerEXT(
	VkInstance instance, 
	VkDebugUtilsMessengerEXT debugMessenger, 
	const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}

void SystemContext::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		                         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		                         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

std::vector<const char*> SystemContext::getRequiredExtensions()
{
	// Get GLFW instance extensions
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef RT_DIST
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	return extensions;
}

std::vector<const char*> SystemContext::checkLayerSupport(std::vector<const char*>& layers)
{
	// Enumerate all supported layers
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	// Vector of wanted supported layers
	std::vector<const char*> supportedLayers{};

	// Check if wanted instance layers are supported
	for (const char* layerName : layers)
	{
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				APP_LOG_TRACE("Layer {} supported", layerName);
				supportedLayers.push_back(layerName);
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
			APP_LOG_ERROR("Layer {} requested, but is not supported", layerName);
	}

	return supportedLayers;
}
