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
#include "Render/Texture.h"
#include "tools/config.h"
#include "Model.h"
#include "Bar.h"
#include "UI/Form.h"

extern float color[3];
extern std::vector<std::string> options;

class Window {
private:
	void initGLFW ();
	void initGLEW ();
	void initImGui ();
	ImGuiWindowFlags flags = 0;
	ImFont* font = nullptr;
public:
	static Form form;
	static Drawable* s;
	static Drawable* focus;
	static bool drawColorPicker;
	static std::vector<Drawable*> managedList;
	GLFWwindow* window;
	static void createOptions (Texture& texture);
	static void buttonActions (int val);
	static void createColorPicker ();
	static void createTooltip ();
	Window();
	void endImGui ();
	void drawImGuiWindow(Texture& texture);
	void draw ();

	void addModelFromForm();

	static void addToRender (Drawable* obj) {
		Window::managedList.insert(Window::managedList.end(), obj);
	}
};
