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
#include "../DecisionTable.h"

enum state {
	PREP = 1,
	FUNCTION,
	INTERVIEW,
	PILOT,
	INTRODUCTION,
};

struct Interview {
	int datapointIndex = 0;
	int categoryIndex = 0;
	std::vector<int> pilotAnswers{};
	std::vector<std::vector<std::vector<int>>> datapoints{};
	std::vector<int> datapoint{};
	int dataPointValue = 1;
	int _class = 0;
	DecisionTable<int> dt{};
};

// multiple functions, rename attributes, word wrapping, load from csv
// figure how closing the window should affect function list

class Form {
private:
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
	int defaultAmount = 15;

	//
	Interview interview{};
	//

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
	void drawInterviewPilot ();
	void drawInterview ();
	void drawIntro ();
	void openWindow ();
	void saveToCSV ();
	void readCSV (std::string path);

	Function* getFunc(void) { return func; }
};
