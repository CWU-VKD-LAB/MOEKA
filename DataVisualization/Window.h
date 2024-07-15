#pragma once
#define GLEW_STATIC
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "tools/config.h"
#include "Shape.h"
#include "ShapeManager.h"

extern float color[3];
extern std::vector<std::string> options;

class Window {
private:
	void initGLFW ();
	void initGLEW ();
public:
	static Shape* s;
	static Shape* focus;
	static bool drawColorPicker;
	static std::vector<Drawable*> managers;
	GLFWwindow* window;
	Window();

	static void addToRender (Drawable* sm) {
		Window::managers.insert(Window::managers.end(), sm);
	}
};
