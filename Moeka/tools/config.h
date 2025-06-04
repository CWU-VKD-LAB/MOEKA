#pragma once
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <imgui_impl_opengl3.h>

namespace config {
	const float buttonSize = 32.f;

	const int maxClassValue = 16;				// hard code to have more possibilities than our slider possibly can, and we can change from here if needed. 
	const int defaultAmount = 15;				// max amount of classes that we could have. 
	
	extern float color[3];
	extern std::vector<std::string> options;
	extern std::vector<ImVec4> classColors;
	extern std::vector<std::vector<std::string>> pilotQuestions;
	extern std::vector<std::string> pilotQuestionDescriptions;
	extern ImVec4 defaultColor;
	extern glm::mat4 proj; 
	extern float windowX;
	extern float windowY;
	extern int drawIndex;

	extern const char* instructions;
}