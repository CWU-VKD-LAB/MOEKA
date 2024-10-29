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
#include <filesystem>

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
	COMPARE,
	LOAD,
	CONSTRAINT
};

// variables directly related to the interview screen.
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

// variables directly related to the comparison screen.
struct Compare {
	int indexOne = 0;
	int indexTwo = 1;
	bool ml = false;
	std::string mlFilePath = "";
	std::string mlFileExtension = "";
	OPENFILENAMEA ofn;
	std::vector<std::vector<int>>* comparisons = nullptr;
};

// constraint screen variables.
struct Constraint {
	std::vector<int> answers{};
};

class Form {
private:
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

	//
	Interview interview{};
	Compare compare{};
	Constraint constraint{};
	//

	// moeka object (files need to be renamed)
	moeka* edm;

	// file reading
	std::vector<std::string> files{};
	int selectedFile = -1;
	std::string basePath = "..\\DataVisualization\\models";


	// what function the program is currently looking at specifically.
	Function* func = nullptr;

	/// @brief flag for synchronization
	bool startMoeka;

	// function that calls moeka->start() with a boolean flag for synchronization
	//void start(bool* flag);

	// function screen variables.
	ImFont* font = nullptr;
	std::string statusMessage = "";
	std::string action = "Add Clause";
	std::vector<std::string> functionNames{};
	std::string currentFunction = "";
	// where the program is currently looking in a function
	int clauseIndex = 0;
	int functionIndex = -1;
	int siblingFunctionIndex = 0;
	int subfunctionIndex = 0;
	bool addedPrevious; // a really stupid variable that fixes a minor bug with moving backwards from the function screen to prep screen.

	// color picker variables
	int colorPickerState = -1;
	bool colorPickerOpen = false;
	bool useGradient = false;
public:
	int current = state::INTRODUCTION;
	static std::vector<Function*> functionList;
	static Function* comparisonFunction;
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
	void drawLoad ();
	void drawContraint ();
	void openWindow ();
	void setNewFunc(std::string = "");
	void compareModelToModel ();
	void compareModelToML ();

	Function* getFunc(void) { return func; }

	Compare getCompare(void ) { return compare; }
};
