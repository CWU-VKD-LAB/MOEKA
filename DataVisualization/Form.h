#pragma once
#include <vector>
#include "tools/config.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <fstream>

enum state {
	PREP = 1,
	FUNCTION,
	INTERVIEW
};

class Form {
private:
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
	int defaultAmount = 20;
	int attributeCount = 1;
	std::vector<int>* clause = nullptr;
	std::vector<std::vector<int>*> history{};
	int current = state::PREP;

	std::vector<int> attributeValueCounts{};
	ImFont* font = nullptr;
	std::string statusMessage = "";
	std::string action = "Add Clause";
public:
	bool open = false;
	Form ();
	~Form ();
	void draw ();
	void drawPrep ();
	void drawFunction ();
	void openWindow ();
};
