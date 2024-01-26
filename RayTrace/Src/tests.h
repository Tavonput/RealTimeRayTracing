#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp> 

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <iostream>
#include <vector>

namespace Tests
{
	void logging();
	void vulkan();
	void glm();
	void glfw();
}