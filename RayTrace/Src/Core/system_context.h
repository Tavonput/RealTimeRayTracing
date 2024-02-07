#pragma once

#include "Application/logging.h"
#include "Application/window.h"

#include "device.h"

class SystemContext
{
public:
	// Initializer
	void init(Window& window, Logger logger);

	// Getters
	const Device& getDevice() const;
	const VkSurfaceKHR& getSurface() const;

	// Cleanup
	void cleanup();

private:
	Device m_device;

	VkSurfaceKHR m_surface = VK_NULL_HANDLE;

	VkInstance               m_instance       = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

	std::vector<const char*> m_instanceLayers;

	Logger m_logger;

	void initInstance();
	void initDebugMessenger();
	void initSurface(Window& window);

	VkResult createDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger);

	void destroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator);

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	std::vector<const char*> getRequiredExtensions();
	void checkLayerSupport();
};
