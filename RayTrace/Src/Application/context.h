#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>
#include <iostream>

#include "logging.h"
#include "window.h"
#include "Core/device.h"

class Context
{
public:
	void init(uint32_t windowWidth, uint32_t windowHeight, Logger logger);

	const Device& getDevice() const;
	const Window& getWindow() const;
	const VkSurfaceKHR& getSurface() const;

	void cleanup();

private:
	Device       m_device;

	Window       m_window;
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;

	VkInstance               m_instance       = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

	std::vector<const char*> m_instanceLayers;

	Logger m_logger;

	void initInstance();
	void initDebugMessenger();
	void initSurface();

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
