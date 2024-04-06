#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class ray {	
	public:
		ray() {}
		ray(const glm::vec3 origin, const glm::vec3 direction);

};