#pragma once
#include <vector>
#include "../tools/config.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include "Function.h"
#include "../moeka/dvector.h"
#include "../DecisionTable.h"
#include "../FileReader.h"

// weird bug if this isn't defined due to some file included in oeka.h
#define NOMINMAX
#include "../moeka/oeka.h"
#undef NOMINMAX

enum state {
	PREP = 1,
	FUNCTION,
	INTERVIEW,
	PILOT,
	INTRODUCTION,
	COLOR,
	COMPARE
};

struct Interview {
	int datapointIndex = 0;
	int hanselChainIndex = 0;
	std::vector<int> pilotAnswers{};
	std::vector<std::vector<std::vector<int>>> datapoints{};
	std::vector<int> datapoint{};
	int dataPointValue = 1;
	int _class = 0;
	DecisionTable<int> dt{};
};

struct Compare {
	int indexOne = 0;
	int indexTwo = 1;
};

class Form {
private:
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

	//
	Interview interview{};
	Compare compare{};
	//

	// moeka object (files need to be renamed)
	moeka* edm;


	//
	Function* func = nullptr;
	std::vector<Function*> functionList{};
	void setNewFunc ();
	/// @brief flag for synchronization
	bool startMoeka;

	// function that calls moeka->start() with a boolean flag for synchronization
	//void start(bool* flag);

	//
	ImFont* font = nullptr;
	std::string statusMessage = "";
	std::string action = "Add Clause";
	std::vector<std::string> functionNames{};
	std::string currentFunction = "";
	int clauseIndex = -1;
	int functionIndex = 0;
	int subfunctionIndex = 0;
	int colorPickerState = -1;
	bool colorPickerOpen = false;
	bool useGradient = false;
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
	void drawColor ();
	void drawCompare ();
	void openWindow ();

	Function* getFunc(void) { return func; }
};
