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
#include "Render/Texture.h"
#include "tools/config.h"

extern float color[3];
extern std::vector<std::string> options;

class Window {
private:
	void initGLFW ();
	void initGLEW ();
	void initImGui ();
	ImGuiWindowFlags flags = 0;
public:
	static Shape* s;
	static Shape* focus;
	static bool drawColorPicker;
	static std::vector<Drawable*> managers;
	GLFWwindow* window;
	static void createTable (Texture& texture);
	static void buttonActions (int val);
	static void createColorPicker ();
	static void createTooltip ();
	Window();
	void endImGui ();
	void drawImGuiWindow(Texture& texture);

	static void addToRender (Drawable* sm) {
		Window::managers.insert(Window::managers.end(), sm);
	}
};
