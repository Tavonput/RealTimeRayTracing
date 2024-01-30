#include "system_context.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	// Debug messenger validation layer callback
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

void SystemContext::init(Window& window, Logger logger)
{
	m_logger = logger;

	initInstance();

#ifdef RT_DEBUG
	initDebugMessenger();
#endif

	initSurface(window);

	m_device.init(m_instance, m_surface, m_instanceLayers, m_logger);
}

const Device& SystemContext::getDevice() const
{
	return m_device;
}

const VkSurfaceKHR& SystemContext::getSurface() const
{
	return m_surface;
}

void SystemContext::cleanup()
{
	LOG_INFO("Destroying context");

#ifdef RT_DEBUG
	destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
#endif

	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

	m_device.cleanup();

	vkDestroyInstance(m_instance, nullptr);
}

void SystemContext::initInstance()
{
	LOG_INFO("Initializing Vulkan instance");

#ifdef RT_DEBUG
	m_instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	// Check if instance layers are supported
	checkLayerSupport();

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
#ifdef RT_DEBUG
	populateDebugMessengerCreateInfo(debugCreateInfo);
	createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#else
	createInfo.pNext = nullptr;
#endif

	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
	{
		LOG_CRITICAL("Failed to create Vulkan instance");
		throw;
	}

	LOG_INFO("Vulkan instance initialization successful");
}

void SystemContext::initDebugMessenger()
{
	LOG_INFO("Initializing debug messenger");

	// Create debug messenger
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (createDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
	{
		LOG_CRITICAL("Failed to create debug messenger");
		throw;
	}

	LOG_INFO("Debug messenger initialization successful");
}

void SystemContext::initSurface(Window& window)
{
	if (glfwCreateWindowSurface(m_instance, window.getWindowGLFW(), nullptr, &m_surface))
	{
		LOG_CRITICAL("Failed to create window surface");
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

#ifdef RT_DEBUG
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	return extensions;
}

void SystemContext::checkLayerSupport()
{
	// Enumerate all supported layers
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	// Check if wanted instance layers are supported
	for (const char* layerName : m_instanceLayers)
	{
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			LOG_CRITICAL("{} not supported", layerName);
			throw;
		}
	}

	return;
}

