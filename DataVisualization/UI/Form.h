#pragma once
#include <vector>
#include "../tools/config.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <fstream>
#include "Function.h"

enum state {
	PREP = 1,
	FUNCTION,
	INTERVIEW
};
// multiple functions, rename attributes, word wrapping, load from csv


// figure how closing the window should affect function list

class Form {
private:
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
	int defaultAmount = 15;
	int current = state::PREP;

	//
	Function* func = nullptr;
	std::vector<Function*> functionList{};
	void setNewFunc ();

	//
	ImFont* font = nullptr;
	std::string statusMessage = "";
	std::string action = "Add Clause";
	std::vector<std::string> functionNames{};
	std::string currentFunction = "";
	int functionIndex = 0;
public:
	bool open = false;
	Form ();
	~Form ();
	void draw ();
	void drawPrep ();
	void drawFunction ();
	void openWindow ();
};
