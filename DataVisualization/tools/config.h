#pragma once
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace config {
	const float windowX = 800;
	const float windowY = 800;
	const float buttonSize = 32.f;
	extern float color[3];
	extern std::vector<std::string> options;
	extern glm::mat4 proj; 
}