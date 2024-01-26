#include "tests.h"

void Tests::logging()
{
    std::cout << "\n** LOGGING TEST **\n------------------\n";

    // Initialize logger
    auto logger = spdlog::stdout_color_mt("LOGGER");
    logger->set_pattern("%^[%T] %n: %v%$");
    logger->set_level(spdlog::level::trace);
    
    // Logging macros. We want to use macros so that we can disable logging from being compiled.
#ifndef RT_DIST
    // Either DEBUG or RELEASE is active. Define logging macros
    #define LOG_TRACE(...)    logger->trace(__VA_ARGS__)
    #define LOG_INFO(...)     logger->info(__VA_ARGS__)
    #define LOG_WARN(...)     logger->warn(__VA_ARGS__)
    #define LOG_ERROR(...)    logger->error(__VA_ARGS__)
    #define LOG_CRITICAL(...) logger->critical(__VA_ARGS__)
#else
    // DIST is active. Set all logging macros to no-operation statements, removing them from compilation.
    #define LOG_TRACE(...)    (void)0
    #define LOG_INFO(...)     (void)0
    #define LOG_WARN(...)     (void)0
    #define LOG_ERROR(...)    (void)0
    #define LOG_CRITICAL(...) (void)0
#endif

    // Test logging
    LOG_TRACE("Trace log");
    LOG_INFO("Info log");
    LOG_WARN("Warn log");
    LOG_ERROR("Error log");
    LOG_CRITICAL("Critical log");

#ifdef RT_DIST
    std::cout << "You should not see any logging here since you are on DIST.\n";
#endif
}

void Tests::vulkan()
{
    std::cout << "\n** VULKAN TEST **\n------------------\n";

    // Check for Vulkan support
    if (!glfwVulkanSupported())
    {
        std::cout << "GLFW: Vulkan not supported\n";
        return;
    }

    // Terminate GLFW because we don't really need it anymore.
    glfwTerminate();

    //
    // Query instance extensions.
    // In Vulkan, there exists things called extensions which are additional features that can be added on.
    // There are two different categories of extensions, instance and device.
    //
    
    // Here I am finding the number of instance extensions supported.
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    // Here I am initializing a vector with its size equal to the number of extensions I found.
    // A vector is pretty much just a dynamic array. Then I am enumerating over all of the instance
    // extensions and storing them in the vector. Calling ".data()" on a vector returns a pointer to
    // the first element of the array.
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    // Print out the number of extensions.
    std::cout << extensionCount << " extensions supported\n";

    // Here I am iterating over all of the instance extensions in the vector and printing out their names.
    for (const auto& extension : extensions)
        std::cout << extension.extensionName << "\n";
}

void Tests::glm()
{
    std::cout << "\n** GLM TEST **\n------------------\n";
    std::cout << "Doing some linear algebra.\n";

    // Test GLM
    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;
}

void Tests::glfw()
{
    std::cout << "** GLFW TEST **\n------------------\n";

    // Initialize glfw
    if (!glfwInit())
    {
        std::cout << "GLFW failed to initialize.\n";
        return;
    }

    std::cout << "Creating and displaying window. Close the window to continue with the tests.\n";

    // Create a window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    // Run window
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    // Cleanup window
    glfwDestroyWindow(window);
}
