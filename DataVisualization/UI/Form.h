#pragma once
#include <vector>
#include "../tools/config.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Function.h"

enum state {
	PREP = 1,
	FUNCTION,
	INTERVIEW,
	INTRODUCTION
};

// multiple functions, rename attributes, word wrapping, load from csv
// figure how closing the window should affect function list

class Form {
private:
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
	int defaultAmount = 15;

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
	int clauseIndex = -1;
	int functionIndex = 0;
	int subfunctionIndex = 0;

	void drawFunctionSelect ();
public:
	int current = state::INTRODUCTION;

	/// @brief add a new model to be drawn by the window
	bool addModel = false;
	bool open = true;
	Form ();
	~Form ();
	void draw ();
	void drawPrep ();
	void drawFunction ();
	void drawIntro ();
	void openWindow ();
	void saveToCSV ();
	void readCSV (std::string path);

	Function* getFunc(void) { return func; }
};
