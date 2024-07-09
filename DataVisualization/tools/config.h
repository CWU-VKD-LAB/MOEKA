#pragma once
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace config {
	const float windowX = 600;
	const float windowY = 600;
	extern float color[3];
	extern std::vector<std::string> options;
	const glm::mat4 proj = glm::ortho(-windowX/2, windowX/2, windowY/2 , -windowY/2, 1.0f, -1.0f);
}