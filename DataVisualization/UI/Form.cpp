#include "Form.h"

Form::Form () {
	interview.dt.readData("test.csv");
	setNewFunc();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	font = io.Fonts->AddFontFromFileTTF("resources/fonts/JetBrainsMono-Medium.ttf", config::windowY / 42.0f);
	interview.pilotAnswers.resize(config::pilotQuestions.size());
	std::fill(interview.pilotAnswers.begin(), interview.pilotAnswers.end(), 0);
}

Form::~Form () {
	for (auto a : functionList) {
		delete(a);
	}
	for (auto a : interview.datapoints) {
		for (auto b : a) {
			delete(&b);
		}
		delete(&a);
	}
}

void Form::draw () {
	if (open) {
		switch (current) {
			case INTRODUCTION:
				drawIntro();
				break;
			case PREP:
				drawPrep();
				break;
			case FUNCTION:
				drawFunction();
				break;
			case PILOT:
				drawInterviewPilot();
				break;
			case INTERVIEW:
				drawInterview();
				break;
			case COLOR:
				drawColor();
				break;
		}
	}
}

void Form::drawIntro () {
	ImGui::Begin("Instructions", nullptr, flags);
	ImGui::PushFont(font);

	ImGui::Text(config::instructions);
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowSize(ImVec2(ImGui::CalcTextSize(config::instructions).x * 1.04f, config::windowY * .2f) );
	ImGui::SetWindowPos( ImVec2(config::windowX * .5f - window.x * .5f, config::windowY * .5f - window.y * .5f) );

	ImGui::SetCursorPosX(window.x * .5f - window.x * .15);
	if (ImGui::Button("OK##", ImVec2(window.x * .3f, window.y *.2f))) {
		open = false;
		current = PREP;
	}
	
	ImGui::PopFont();
	ImGui::End();
}

void Form::drawPrep () {
	// create the window with a slider at the top
	ImGui::Begin("Prep Window", &open, flags);
	ImGui::PushFont(font);
	ImVec2 window = ImGui::GetWindowSize();

	// header
	ImGui::InputText("##", func->functionName, 128);
	ImGui::SameLine(window.x * .7575f);
	drawFunctionSelect();
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x * .5f - (ImGui::CalcTextSize("Amount of Target Attributes: ").x * .5f));
	ImGui::Text("Amount of Target Attributes: ");
	ImGui::SetNextItemWidth(ImGui::GetWindowSize().x * .96f);
	ImGui::SliderInt("##amtTargetSlider", &func->targetAttributeCount, 2, defaultAmount);
	ImGui::Separator();

	ImGui::SetCursorPosX(ImGui::GetWindowSize().x * .5f - (ImGui::CalcTextSize("Amount of Attributes: ").x * .5f));
	ImGui::Text("Amount of Attributes: ");
	ImGui::SetNextItemWidth(ImGui::GetWindowSize().x * .96f);
	ImGui::SliderInt("##amtSlider", &func->attributeCount, 2, defaultAmount);
	ImGui::Separator();

	// create a subwindow that contains the attributes
	ImGui::BeginChild("##", ImVec2{ ImGui::GetWindowSize().x * .95f, ImGui::GetWindowSize().y * .5f }, ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysVerticalScrollbar);
	for (int a = 0; a < func->attributeCount; a++) {
		ImGui::SetNextItemWidth(ImGui::GetWindowSize().x * .28f);
		ImGui::InputText(std::string("##Text").append(std::to_string(a)).c_str(), func->attributeNames.at(a), 128);
		ImGui::SetNextItemWidth(ImGui::GetWindowSize().x * .66f);
		ImGui::SameLine(ImGui::GetWindowSize().x * .3f);
		ImGui::SliderInt(std::string("##").append(std::to_string(a)).c_str(), 
			&func->kValues.data()[a], 2, 10);
	}
	ImGui::PopFont();
	ImGui::EndChild();
	//

	

	// create the bottom section that contains the enter button.
	ImGui::Separator();
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x * .34f);
	ImVec2 buttonSize{ ImGui::GetWindowSize().x * .2f, ImGui::GetWindowSize().y * .1f };

	// creates the skip button, if function list is empty however, disable it.
	if (functionList.empty()) {
		ImGui::BeginDisabled();
	}
	if (ImGui::Button("Skip", buttonSize)) {
		current = FUNCTION;
		if (functionIndex > functionList.size()-1) {
			functionIndex--;
		}
		func = functionList.at(functionIndex);
	}
	if (functionList.empty()) {
		ImGui::SetItemTooltip("Must create a function first.");
		ImGui::EndDisabled();
	}

	ImGui::SameLine();
	if (ImGui::Button("Function", buttonSize)) {		
		current = FUNCTION;
		std::vector<int>* temp = new std::vector<int>;
		temp->resize(func->attributeCount);
		std::fill(temp->begin(), temp->end(), 0);
		func->clause = temp;
		if (std::count(functionList.begin(), functionList.end(), func) == 0) {
			functionList.insert(functionList.end(), func);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Interview", buttonSize)) {		
		current = PILOT;
		std::vector<int>* temp = new std::vector<int>;
		temp->resize(func->attributeCount);
		std::fill(temp->begin(), temp->end(), 0);
		func->clause = temp;
		if (std::count(functionList.begin(), functionList.end(), func) == 0) {
			functionList.insert(functionList.end(), func);
		}

		//
		interview.datapoints.resize(func->subfunctionList.size());
		for (int a = 0; a < interview.datapoints.size(); a++) {
			std::vector<std::vector<int>>* category = new std::vector<std::vector<int>>;
			category->resize(interview.datapoints.size());
			//
			for (int b = 0; b < category->size(); b++) {
				std::vector<int>* datapoint = new std::vector<int>;
				category->at(a) = *datapoint;
			}
			interview.datapoints[a] = *category;
		}
		//
	}
	

	// set window size and position
	ImGui::SetWindowSize(ImVec2(config::windowX * .5f, config::windowY * .5f));
	ImGui::SetWindowPos(ImVec2(window.x - (config::windowX * .25f), window.y - (config::windowY * .25f)));
	ImGui::End();
}

// draws the function screen where the user can input clauses and save to file.
void Form::drawFunction () {
	ImGui::Begin("##", &open, flags);
	ImGui::SetWindowSize(ImVec2(config::windowX * .75f, config::windowY * .75f));
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2(window.x - (config::windowX * .625f), window.y - (config::windowY * .625f)));

	//// header
	// back button
	
	if (ImGui::Button("Back", ImVec2{ window.x * .15f, window.y * .05f })) {
		current = PREP;
	}
	ImGui::SameLine();
	// create clause header
	ImGui::SetCursorPosX(window.x * .4f - (ImGui::CalcTextSize("Create a Clause").x * .5f));
	ImGui::PushFont(font);
	ImGui::Text("Create a Clause");
	ImGui::SameLine(window.x * .835f - ImGui::CalcTextSize(func->functionName).x);
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(50, 50, 255, 255));
	ImGui::Text(func->functionName);
	ImGui::PopStyleColor();
	ImGui::SameLine(window.x * .835f);
	drawFunctionSelect();

	ImGui::Separator();

	// value setting field
	ImGui::BeginChild("##subwindow", ImVec2{ window.x * .95f, window.y * .61f }, ImGuiChildFlags_None);
	ImGui::SetNextItemWidth(100);
	if (ImGui::BeginTable("##functiontable", func->attributeCount, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollX | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_BordersOuterV)) {
		for (int a = 0; a < func->attributeCount; a++) {
			ImGui::TableSetupColumn(func->attributeNames.at(a), 160);
		}
		ImGui::TableHeadersRow();
		for (int a = 0; a < func->attributeCount; a++) {
			ImGui::TableNextColumn();
			for (int b = 0; b < func->kValues.at(a); b++) {
				ImGui::RadioButton(std::to_string(b).append("##").append(std::to_string(a)).c_str(), &func->clause->data()[a], b);
			}
		}
		ImGui::EndTable();
	}
	
	ImGui::EndChild();
	ImGui::PopFont();
	ImGui::Separator();
	//
	
	float test = ImGui::GetCursorPos().y; // for buttons later
	if (ImGui::BeginTable("##existing_clauses", (int)(func->subfunctionList[subfunctionIndex].size() + 1), ImGuiTableFlags_ScrollX | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV, ImVec2{ window.x * .67f, window.y * .13f })) {
		for (int a = 0; a < func->subfunctionList[subfunctionIndex].size(); a++) {
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(100);
			ImGui::Text(std::string("Clause ").append(std::to_string(a + 1)).c_str());
			ImGui::SameLine(85);

			bool disabled = a == clauseIndex;
			if (ImGui::Button(std::string("x##").append(std::to_string(a)).c_str(), ImVec2{ 15.0f, 15.0f })) {
				func->subfunctionList[subfunctionIndex].erase(func->subfunctionList[subfunctionIndex].begin() + a);
				statusMessage = std::string("Removed Clause ").append(std::to_string(a + 1));
				action = "Add Clause";
				if (disabled) {
					if (clauseIndex > func->subfunctionList[subfunctionIndex].size()) {
						clauseIndex--;
					}
					else {
						clauseIndex = 0;
					}
					func->clause = func->subfunctionList[subfunctionIndex].at(clauseIndex);
				}
			}
			// if we already have this clause loaded, disable the load button.
			if (disabled) {
				ImGui::BeginDisabled();
			}
			if (ImGui::Button(std::string("Load##").append(std::to_string(a)).c_str(), ImVec2{ 100, 38 })) {
				func->clause = func->subfunctionList[subfunctionIndex].at(a);
				statusMessage = std::string("Loaded Clause ").append(std::to_string(a + 1));
				action = "Update Clause";
				clauseIndex = a;
			}
			if (disabled) {
				ImGui::EndDisabled();
			}
		}
		ImGui::EndTable();
	}
	

	// functions table
	if (ImGui::BeginTable("##subfunction", (int)(func->subfunctionList.size() + 1), ImGuiTableFlags_ScrollX | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV, ImVec2{ window.x * .67f, window.y * .13f })) {
		for (int a = 0; a < func->subfunctionList.size(); a++) {
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(100);
			ImGui::Text(std::string("Sub-Func ").append(std::to_string(a + 1)).c_str());
			ImGui::SameLine(85);
			if (ImGui::Button(std::string("x##").append(std::to_string(a)).c_str(), ImVec2{ 15.0f, 15.0f })) {
				func->subfunctionList.erase(func->subfunctionList.begin() + a);
				statusMessage = std::string("Removed Sub ").append(std::to_string(a + 1));
				if (subfunctionIndex >= func->subfunctionList.size()) {
					subfunctionIndex--;
				}
			}
			if (ImGui::Button(std::string("Load##").append(std::to_string(a)).c_str(), ImVec2{ 100, 38 })) {
				statusMessage = std::string("Loaded Sub ").append(std::to_string(a + 1));
				subfunctionIndex = a;
			}
		}
		ImGui::EndTable();
	}
	

	//// options
	ImVec2 buttonSize{ window.x * .3f, window.y * .05f };

	// 
	float yPos = test;
	float xPos = window.x * .99f - buttonSize.x;
	float stride = 2.5f;
	ImGui::SetCursorPosX(xPos);
	ImGui::SetCursorPosY(yPos);
	if (ImGui::Button("Add a sub function", buttonSize)) {
		std::vector<std::vector<int>*> subfunction;
		func->subfunctionList.insert(func->subfunctionList.end(), subfunction);
		subfunctionIndex++;
		statusMessage = "Added sub function";
	}

	// new parent function
	ImGui::SetCursorPosX(xPos);
	yPos += buttonSize.y + stride;
	ImGui::SetCursorPosY(yPos);
	if (ImGui::Button("Add a function", buttonSize)) {
		setNewFunc();
		functionIndex++;
		current = PREP;
		subfunctionIndex = 0;
		action = "Add Clause";
	}

	// status message. the random 0.00025f * window.y shifts it an extra little bit but is entirely optional. it just bothered me it was slightly off.
	yPos += buttonSize.y + stride + window.y * 0.00025f;
	if (!statusMessage.empty()) {
		ImGui::PushFont(font);
		ImVec2 shift = ImGui::CalcTextSize(statusMessage.c_str());
		ImGui::SetCursorPosX(window.x * .99f - shift.x - (buttonSize.x - shift.x) * .5f);
		ImGui::SetCursorPosY(yPos + shift.y * .25f);
		ImGui::Text(statusMessage.c_str(), buttonSize);
		ImGui::PopFont();
	}

	ImGui::SetCursorPosX(xPos);
	yPos += (buttonSize.y + stride);
	ImGui::SetCursorPosY(yPos);

	// add clause button
	if (ImGui::Button(action.c_str(), buttonSize)) {
		if (action == "Add Clause") {
			statusMessage = "Added Clause";
			
			if (func->subfunctionList.empty())
			{
				func->subfunctionList.push_back({});
			}
			func->subfunctionList[subfunctionIndex].insert(func->subfunctionList[subfunctionIndex].end(), func->clause);
		}
		else if (action == "Update Clause") {
			statusMessage = "Updated Clause";
		}

		func->clause = new std::vector<int>{};
		func->clause->resize(func->attributeCount);
		std::fill(func->clause->begin(), func->clause->end(), 0);
		action = "Add Clause";
		// TODO check for dupes?
	}
	
	// finish button
	ImGui::SetCursorPosX(xPos);
	yPos += buttonSize.y + stride;
	ImGui::SetCursorPosY(yPos);

	if (ImGui::Button("Finish", buttonSize)) {
		statusMessage = "Pressed Finish Button";
		// TODO save to file screen
		saveToCSV();
		open = !open;

		//create hanselChainSet for function
		func->initializeHanselChains();

		// organize them and assign classes such that we can visualize
		func->setUpHanselChains();

		// create a model for the hanselChains
		addModel = true;
	}

	//
	ImGui::End();
}

void Form::drawInterviewPilot () {
	// TO DO : add stuff for decision table for pilot questions
	ImGui::Begin("##", &open, flags);
	ImGui::SetWindowSize(ImVec2(config::windowX * .75f, config::windowY * .5f));
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2(window.x - (config::windowX * .625f), window.y - (config::windowY * .25f)));
	std::string question;

	//
	ImGui::PushFont(font);
	std::string temp;
	for (int b = 0; b < config::pilotQuestions.size(); b++) {
		if (config::pilotQuestions[b][0][config::pilotQuestions[b][0].size() - 2] == 's') {
			question = "What are the ";
		}
		else {
			question = "What is the ";
		}
		question.append(config::pilotQuestions[b][0]);

		
		ImGui::SetCursorPosX(window.x * .5f - ImGui::CalcTextSize(question.c_str()).x * .5f - (ImGui::CalcTextSize("(?)").x * .5f));
		//
		ImGui::TextDisabled("(?)");
		if (ImGui::BeginItemTooltip())
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(config::pilotQuestionDescriptions[b].c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		//
		ImGui::SameLine();
		ImGui::Text(question.c_str());

		ImGui::Separator();
		
		if (ImGui::BeginTable(std::string("##Question ").append(std::to_string(b)).c_str(), config::pilotQuestions[b].size() - 1, ImGuiTableFlags_ScrollX | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV, ImVec2{ window.x, window.y * .195f })) {
			for (int a = 1; a < config::pilotQuestions[b].size(); a++) {
				ImGui::TableNextColumn();
				temp = config::pilotQuestions[b][a];
				ImGui::RadioButton(temp.append("##").append(std::to_string(b)).c_str(), &interview.pilotAnswers[b], a);
			}
			ImGui::EndTable();
		}
	}
	ImGui::Separator();
	ImGui::PopFont();
	//


	//
	ImVec2 buttonSize{window.x * .1f, window.y * .1f};
	ImGui::SetCursorPosX(window.x * .9875f - buttonSize.x);
	bool disable = false;
	for (auto a : interview.pilotAnswers) {
		if (a == 0) {
			disable = true;
			break;
		}
	}
	if (disable) {
		ImGui::BeginDisabled();
	}
	if (ImGui::Button("Next##", buttonSize)) {
		// this doesnt do anything really - need to change 
		func->initializeHanselChains();
		interview.datapoints = func->hanselChains->hanselChainSet;
		interview.datapoint.resize(func->clause->size());
		current = state::INTERVIEW;
	}
	if (disable) {
		ImGui::EndDisabled();
	}
	//

	ImGui::End();
}

void Form::drawInterview () {
	ImGui::Begin("##", &open, flags);
	ImGui::SetWindowSize(ImVec2(config::windowX * .75f, config::windowY * .2f));
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2(window.x - (config::windowX * .625f), config::windowY * .4f));
	ImGui::PushFont(font);

	//// TODO: replace with actual datapoint.
	
	// below obsolete now
	//std::vector<int> data{};
	//data.resize(interview.datapoint.size());
	
	//create hanselChainSet for function
	// is this done every frame
	auto& datapoint = interview.datapoints[interview.categoryIndex][interview.datapointIndex];

	////

	ImGui::SetCursorPosX(window.x * .5f - ImGui::CalcTextSize("Input a class for this datapoint.").x * .5f);
	ImGui::Text("Input a target value (class) for the datapoint.");
	ImGui::SetNextItemWidth(window.x * .2f);
	ImGui::Separator();
	ImGui::Text("Datapoint: ");
	for (int a = 0; a < datapoint.size(); a++) {
		ImGui::SameLine();
		ImGui::SetNextItemWidth(font->FontSize);
		ImGui::Text(std::to_string(datapoint[a]).c_str());
	}
	ImGui::Text("Target Value (Class): ");
	for (int b = 0; b < interview.datapoint.size(); b++) {
		ImGui::SameLine();
		ImGui::RadioButton(std::to_string(b).append("##").append(std::to_string(b)).c_str(), &interview._class, b);
	}
	ImGui::PopFont();
	//
	ImGui::Separator();
	//
	ImGui::SetCursorPosY(window.y * 0.75f);
	ImVec2 buttonSize{ window.x * .2f, window.y * .2f };
	ImGui::SetCursorPosX(window.x * .56f - buttonSize.x);

	// below  dont do anything need to change 
	if (ImGui::Button("Next##", buttonSize)) {
		// add monotonic expansion
		// increment intra chain
		if (interview.datapointIndex < interview.datapoints[interview.categoryIndex].size() - 1)
		{
			interview.datapointIndex++;
		}
		// incrememnt chain
		else if (interview.categoryIndex < interview.datapoints.size() - 1)
		{
			interview.categoryIndex++;
			interview.datapointIndex = 0;
		}
		// else interview is done
		else
		{
			action = state::PREP;
			open = !open;
		}
	}

	// temporarily disable
	/*
	ImGui::SameLine();
	if (ImGui::Button("Add Datapoint##", buttonSize)) {
		interview.datapoints[interview.categoryIndex][interview.datapointIndex].push_back(interview.dataPointValue);

		// debug
		std::cout << "------------" << std::endl;
		std::cout << "function" << std::endl;
		for (auto a : interview.datapoints) {
			std::cout << " categories" << std::endl;
			for (auto b : a) {
				std::cout << "  ----------\n  datapoints" << std::endl;
				for (auto c : b) {
					std::cout << "  " << c << std::endl;
				}
			}
		}
		//
	}*/
	ImGui::SameLine();
	if (ImGui::Button("Done##", buttonSize)) {
		action = state::PREP;
		open = !open;
	}
	//

	ImGui::End();
}

void Form::openWindow () {
	open = true;
}

void Form::drawColor () {
	ImGui::Begin("Set the Colors of each Class Value##ClassColors", &open, flags);
	ImGui::SetWindowSize(ImVec2(config::windowX * .75f, config::windowY * .4f));
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2(config::windowX * .5f - window.x * .5f, config::windowY * .5f - window.y * .5f));

	float paddingX = ImGui::GetStyle().WindowPadding.x * 2.0f;
	float itemSpacingX = ImGui::GetStyle().ItemSpacing.x;
	float size = (window.x - paddingX - (itemSpacingX * (config::classColors.size()-1)) ) / config::classColors.size();
	ImVec2 item{size, size};
	ImVec4 col;

	ImGui::Checkbox("Use Gradient Instead?", &useGradient);
	ImGui::SeparatorText("Gradient");
	//
	if (!useGradient) {
		ImGui::BeginDisabled();
	}
	
	col = config::classColors[0];
	ImGui::Text("Gradient Start: ");
	ImGui::SameLine();
	ImGui::SetCursorPosX(window.x * .2f);
	ImGui::Text("Gradient End: ");
	ImGui::PushStyleColor(ImGuiCol_Button, col);
	if (ImGui::Button("##0", item)) {
		colorPickerState = 0;
		colorPickerOpen = true;
	}
	ImGui::SameLine();
	ImGui::PopStyleColor();
	col = config::classColors[config::classColors.size()-1];
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, col);
	ImGui::SetCursorPosX(window.x * .2f);
	if (ImGui::Button("##1", item)) {
		colorPickerState = config::classColors.size()-1;
		colorPickerOpen = true;
	}
	ImGui::PopStyleColor();
	ImGui::SeparatorText("Class Colors:");

	if (!useGradient) {
		ImGui::EndDisabled();
	}
	//

	//
	if (useGradient) {
		ImGui::BeginDisabled();
	}
	ImGui::PushFont(font);
	std::string base = "Class ";
	for (int a = 0; a < config::classColors.size(); a++) {
		col = config::classColors[a];
		ImGui::PushStyleColor(ImGuiCol_Button, col);
		if (ImGui::Button((base + std::to_string(a + 1)).c_str(), item)) {
			colorPickerState = a;
			colorPickerOpen = true;
		}
		ImGui::SameLine();
		ImGui::PopStyleColor();
	}
	ImGui::PopFont();
	if (useGradient) {
		ImGui::EndDisabled();
	}
	//
	
	//
	if (colorPickerOpen) {
		ImGui::Begin("Color Picker", &colorPickerOpen);
		ImGui::SetWindowFocus();
		ImVec2 colorPickerSize{ config::windowX * .3f, config::windowY * .3f };
		ImGui::SetWindowSize(colorPickerSize);
		ImGui::SetWindowPos(ImVec2(config::windowX * .5f - colorPickerSize.x * .5, config::windowY * .5f - colorPickerSize.y * .5));
		ImGui::SetNextItemWidth(colorPickerSize.x -(ImGui::GetStyle().WindowPadding.x * 2.0f));
		if (ImGui::ColorPicker4((base + std::to_string(colorPickerState + 1)).c_str(), (float*)&config::classColors[colorPickerState], ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoLabel)) {
			if (useGradient) {
				// change in rgb
				int end = config::classColors.size() - 1;
				float dr = (config::classColors[end].x - config::classColors[0].x) / end;
				float dg = (config::classColors[end].y - config::classColors[0].y) / end;
				float db = (config::classColors[end].z - config::classColors[0].z) / end;
				for (int a = 0; a < config::classColors.size(); a++) {
					config::classColors[a].x = config::classColors[0].x + dr * a;
					config::classColors[a].y = config::classColors[0].y + dg * a;
					config::classColors[a].z = config::classColors[0].z + db * a;
				}
			}
			
		}
		ImGui::End();
	}

	ImGui::End();
}

// when we click to add a new function, or open the program for the first time, this creates a function in memory.
void Form::setNewFunc () {
	char* name = new char[128];
	strcpy_s(name, 128, std::string("Function ").append(std::to_string(functionList.size() + 1)).c_str());
	func = new Function(name);
	func->kValues.resize(defaultAmount); // TODO: needs to be the actual number of values
	func->attributeNames.resize(defaultAmount);
	// set default names of attributes
	int b = 0;
	for (auto a : func->attributeNames) {
		char* buff = new char[128];
		strcpy_s(buff, 128, std::string("Attribute ").append(std::to_string(b+1)).c_str() );
		func->attributeNames.at(b) = buff;
		b++;
	}
	//
	std::fill(func->kValues.begin(), func->kValues.end(), 2);

	if (func->subfunctionList.empty())
	{
		func->subfunctionList.push_back({});
	}

}

// saves the contents of the functionList and its related clauses to a CSV file.
void Form::saveToCSV () {
	// todo: update to support child functions
	//std::ofstream file("output.csv");
	//Function* f;
	//// for each function in memory
	//for (int a = 0; a < functionList.size(); a++) {
	//	f = functionList.at(a);
	//	file << "#" << f->functionName << std::endl;
	//	// write the attribute names
	//	for (int b = 0; b < f->attributeCount; b++) {
	//		file << f->attributeNames.at(b) << ", ";
	//	}
	//	file << std::endl;

	//	// write the range of kValues
	//	for (int c = 0; c < f->kValues.size(); c++) {
	//		file << f->kValues.at(c) << ", ";
	//	}
	//	file << std::endl;

	//	// for each clause
	//	for (int c = 0; c < f->subfunctionList.size(); c++) {
	//		for (int d = 0; d < f->subfunctionList[0].at(c)->size(); d++) {
	//			file << f->subfunctionList[0].at(c)->at(d) << ", ";
	//		}
	//		file << std::endl;
	//	}
	//	file << std::endl;
	//}
	//file.close();
}

// this draws the 2 arrows for swapping between functions.
void Form::drawFunctionSelect () {
	ImGui::Text((currentFunction + " " + std::to_string(functionIndex + 1) + "/" + std::to_string(functionList.size())).c_str());
	ImGui::SameLine();
	if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
		if (functionIndex > 0) {
			functionIndex--;
			func = functionList.at(functionIndex);
			subfunctionIndex = 0;
			action = "Add Clause";
		}
	}
	ImGui::SameLine();
	if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
		if (functionIndex < functionList.size() - 1) {
			functionIndex++;
			func = functionList.at(functionIndex);
			subfunctionIndex = 0;
			action = "Add Clause";
		}
	}
}

void Form::readCSV (std::string path) {
	// TODO update to support child functions
	std::ifstream file(path);
	std::string line;
	std::string delimiter = ", ";
	std::string token;
	std::vector<Function*> tempFuncList;
	Function* current = nullptr;
	bool clauseDefined = false;
	while (std::getline(file, line)) {
		if (line.empty()) {
			continue;
		}
		
		// the start of a new function is denoted by a #
		if (line.at(0) == '#') {
			char* tempString = new char[128];
			strcpy_s(tempString, 128, line.substr(1, line.length()).c_str());
			current = new Function(tempString);
			clauseDefined = false;
			continue;
		}

		if (current == nullptr) {
			continue;
		}

		// body of a function, maybe find better way of testing.
		if (!std::isdigit(line.at(0))) {
			// names of the attributes.
			while (!line.empty()) {
				token = line.substr(0, line.find(delimiter));
				line.erase(0, line.find(delimiter) + delimiter.length());
				// the warning given below should not happen, given a properly saved csv.
				char* tempString = new char[128];
				strcpy_s(tempString, 128, token.c_str());
				current->attributeNames.insert(current->attributeNames.end(), tempString);
			}
			current->attributeCount = (int)current->attributeNames.size();
			current->kValues.resize(current->attributeCount);
			tempFuncList.insert(tempFuncList.end(), current);
		}
		else {
			int index = 0;
			int val;
			std::vector<int>* tempClause = new std::vector<int>;
			while (!line.empty()) {
				token = line.substr(0, line.find(delimiter));
				line.erase(0, line.find(delimiter) + delimiter.length());
				val = std::stoi(token);
				if (!clauseDefined) {
					current->kValues.at(index) = val;
					index++;
					continue;
				}
				//current->kValues.at(index) = std::max(current->kValues.at(index), val);
				tempClause->insert(tempClause->end(), val);
				index++;
			}
			if (!clauseDefined) {
				clauseDefined = true;
				continue;
			}
			current->subfunctionList[0].insert(current->subfunctionList[0].end(), tempClause);
		}
	}
	functionList.swap(tempFuncList);
	func = functionList.at(0);
	if (func->subfunctionList.size() != 0) {
		func->clause = func->subfunctionList[0].at(0);
	}
	else {
		std::vector<int>* temp = new std::vector<int>;
		temp->resize(func->attributeCount);
		std::fill(temp->begin(), temp->end(), 0);
		func->clause = temp;
	}
	file.close();
}
