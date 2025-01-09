#include "Form.h"

std::vector<Function*> Form::functionList{};
Function* Form::comparisonFunction = nullptr;

// Define the global variables here (allocate memory)
int classCount = 16;                      
std::vector<char*> classNames;

// creates a form object, that acts as a root for all the form screens. a list of the managed screens is under the 
// 'state' enumeration in form.h 
Form::Form () {
	// reads interview questions and stores them for later.
	interview.dt.readData("test.csv");
	interview.pilotAnswers.resize(config::pilotQuestions.size());
	std::fill(interview.pilotAnswers.begin(), interview.pilotAnswers.end(), 0);

	// set a default function on form creation
	setNewFunc();
	func->trueAttributes = new std::vector<int>(func->attributeCount);

	// load font
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	font = io.Fonts->AddFontFromFileTTF("resources/fonts/JetBrainsMono-Medium.ttf", config::windowY / 42.0f);
	interview.pilotAnswers.resize(config::pilotQuestions.size());
	std::fill(interview.pilotAnswers.begin(), interview.pilotAnswers.end(), 0);
	func->trueAttributes = new std::vector<int>(func->attributeCount, -1);


	// synchronization flag
	startMoeka = new bool(true);

	// load all files from the models folder
	for (auto a : std::filesystem::directory_iterator(basePath)) {
		files.push_back(a.path().string().erase(0, basePath.length() + 1));
	}
}

// destructor for the form, deleting heap allocated variables.
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
	delete (startMoeka);
}

// base for drawing the form, which form screen it renders is dependent on the 'current' 
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
			case COMPARE:
				drawCompare();
				break;
			case LOAD:
				drawLoad();
				break;
			case CONSTRAINT:
				//drawContraint();
				break;
			case ML:
				drawML();
				break;
		}
	}
}

/// creates a window that pops up when we load into the program that shows the keybinds and an option to open up the help document.
void Form::drawIntro () {
	ImGui::Begin("Instructions", nullptr, flags);
	ImGui::PushFont(font);

	ImGui::Text(config::instructions);
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowSize(ImVec2(ImGui::CalcTextSize(config::instructions).x * 1.04f, config::windowY * .2f) );
	ImGui::SetWindowPos( ImVec2(config::windowX * .5f - window.x * .5f, config::windowY * .5f - window.y * .5f) );
	ImVec2 size{ImGui::CalcTextSize("Open Help Document").x, ImGui::GetFontSize()};
	ImGui::SetCursorPos(ImVec2{window.x * .5f - size.x * .5f, ImGui::GetCursorPosY() - ImGui::GetFontSize()});
	if (ImGui::Button("Open Help Document", size)) {
		std::filesystem::path p = std::filesystem::current_path() / "../HelpDocument.docx";

		std::cout << ShellExecuteA(NULL, "open", p.string().c_str(), NULL, NULL, SW_SHOWNORMAL) << std::endl;
	}
	ImGui::SetCursorPos(ImVec2{window.x * .5f - window.x * .15f, window.y - (window.y * .2f) - ImGui::GetStyle().WindowPadding.x});
	if (ImGui::Button("OK##", ImVec2(window.x * .3f, window.y *.2f))) {
		open = false;
		current = PREP;
	}
	ImGui::PopFont();
	ImGui::End();
}

/// window for setting an ML and dataset
void Form::drawML () {
	ImGui::Begin("Create from ML", &open, flags);
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowSize(ImVec2(config::windowX * .5f, config::windowY * .35f));
	ImGui::SetWindowPos(ImVec2(config::windowX * .5f - window.x * .5f, config::windowY * .5f - window.y * .5f));
	ImGui::PushFont(font);


	if (ImGui::BeginCombo("ML model", ml.mlFilePaths[ml.mlIndex].c_str())) {
		for (int a = 0; a < ml.mlFilePaths.size(); a++) {
			if (ImGui::Selectable(ml.mlFilePaths[a].c_str(), ml.mlIndex == a)) {
				ml.mlSelected = ml.mlFilePaths[a];
				ml.mlIndex = a;
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SetCursorPosX(window.x * .2f);
	if (ImGui::Button("Import external model...")) {
		char sizeOfFilePath[300];
		ZeroMemory(&ml.ofn, sizeof(OPENFILENAME));
		ml.ofn.lStructSize = sizeof(OPENFILENAME);
		ml.ofn.hwndOwner = NULL;
		ml.ofn.lpstrFile = sizeOfFilePath;
		ml.ofn.lpstrFile[0] = '\0';
		ml.ofn.nFilterIndex = 1;
		ml.ofn.nMaxFile = sizeof(sizeOfFilePath);
		ml.ofn.lpstrFileTitle = NULL;
		ml.ofn.nMaxFileTitle = 0;
		ml.ofn.lpstrInitialDir = NULL;
		ml.ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		GetOpenFileNameA(&ml.ofn);
		ml.mlSelected = ml.ofn.lpstrFile;
	}

	if (ImGui::BeginCombo("Dataset", ml.dFilePaths[ml.dIndex].c_str())) {
		for (int a = 0; a < ml.dFilePaths.size(); a++) {
			if (ImGui::Selectable(ml.dFilePaths[a].c_str(), ml.dIndex == a)) {
				ml.dSelected = ml.dFilePaths[a];
				ml.dIndex = a;
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();

	// set K Values.
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x * .5f - (ImGui::CalcTextSize("Amount of Attributes: ").x * .5f));
	ImGui::Text("Amount of Attributes: ");
	ImGui::SetNextItemWidth(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2.0f);
	ImGui::SliderInt("##amtSlider", &func->attributeCount, 2, config::defaultAmount);

	ImGui::BeginChild("##", ImVec2{ ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2.0f, window.y - ImGui::GetCursorPosY() - (font->FontSize * 2) }, ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysVerticalScrollbar);
	for (int a = 0; a < func->attributeCount; a++) {
		ImGui::SetNextItemWidth(ImGui::GetWindowSize().x * .28f);
		ImGui::InputText(std::string("##Text").append(std::to_string(a)).c_str(), func->attributeNames.at(a), 128);
		ImGui::SetNextItemWidth(ImGui::GetWindowSize().x * .66f);
		ImGui::SameLine(ImGui::GetWindowSize().x * .3f);
		ImGui::SliderInt(std::string("##").append(std::to_string(a)).c_str(),
			&func->kValues.data()[a], 2, 10);
	}
	ImGui::EndChild();
	ImGui::Separator();


	ImGui::SetCursorPosX(window.x * .8f - ImGui::GetStyle().WindowPadding.x);
	if (ImGui::Button("Next##asdf", ImVec2{window.x * .2f, (font->FontSize * 2) - (ImGui::GetStyle().WindowPadding.y * 2.0f)})) {
		// Logic goes here.
	}

	ImGui::PopFont();
	ImGui::End();
}

/// Prep window for before we go to the function screen or interview screen.
void Form::drawPrep () {
	// create the window with a slider at the top
	ImGui::Begin("Prep Window", &open, flags);
	ImGui::PushFont(font);
	ImVec2 window = ImGui::GetWindowSize();

	// set window size and position
	ImGui::SetWindowSize(ImVec2(config::windowX * .6f, config::windowY * .5f));
	ImGui::SetWindowPos(ImVec2(window.x - (config::windowX * .4f), window.y - (config::windowY * .25f)));

	// header
	float positionY = ImGui::GetCursorPosY();
	ImGui::InputText("##", func->functionName, 128);
	ImGui::SameLine();
	
	// function swapping
	std::string tempText = currentFunction + " " + std::to_string(functionIndex + 1) + "/" + std::to_string(functionList.size());
	ImGui::SetCursorPos(ImVec2{ImGui::GetWindowSize().x - ImGui::CalcTextSize(tempText.c_str()).x - font->FontSize * 2.0f - ImGui::GetStyle().ItemSpacing.x * 3.0f, positionY});
	ImGui::Text(tempText.c_str());
	ImGui::SameLine();
	// move left in function list
	if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
		if (functionIndex > 0) {
			functionIndex--;
			func = functionList.at(functionIndex);
			siblingFunctionIndex = 0;
			clauseIndex = 0;
			action = "Add Clause";
		}
	}
	// move right in function list
	ImGui::SameLine();
	if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
		if (functionIndex < (int)(functionList.size() - 1) ) {
			functionIndex++;
			func = functionList.at(functionIndex);
			siblingFunctionIndex = 0;
			clauseIndex = 0;
			action = "Add Clause";
		}
	}

	// target attributes amount slider
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x * .5f - (ImGui::CalcTextSize("Amount of Target Attributes: ").x * .5f));
	ImGui::Text("Amount of Target Attributes: ");
	ImGui::SetNextItemWidth(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2.0f);
	ImGui::SliderInt("##amtTargetSlider", &func->targetAttributeCount, 2, config::defaultAmount);

	// Target attributes names list
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x * .5f - (ImGui::CalcTextSize("Target Attribute Names: ").x * .5f));
	ImGui::Text("Target Attribute Names: ");
	ImGui::SetNextItemWidth(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2.0f);
	
	// Create a subwindow to list target classes
	ImGui::BeginChild("##TargetClasses",
		ImVec2{ ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2.0f, ImGui::GetWindowSize().y * .4f },
		ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysVerticalScrollbar);

	for (int t = 0; t < func->targetAttributeCount; t++) {
		// InputText for naming target classes
		ImGui::SetNextItemWidth(ImGui::GetWindowSize().x * .9f); // Adjust width as needed
		ImGui::InputText(std::string("##TargetName").append(std::to_string(t)).c_str(), func->targetAttributeNames.at(t), 128);
	}

	ImGui::EndChild();

	// amount of attributes
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x * .5f - (ImGui::CalcTextSize("Amount of Attributes: ").x * .5f));
	ImGui::Text("Amount of Attributes: ");
	ImGui::SetNextItemWidth(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2.0f);
	ImGui::SliderInt("##amtSlider", &func->attributeCount, 2, config::defaultAmount);
	ImGui::Separator();

	// create a subwindow that contains the attributes
	ImGui::BeginChild("##", ImVec2{ ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2.0f, ImGui::GetWindowSize().y * .5f }, ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysVerticalScrollbar);
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
	ImVec2 buttonSize{ ((ImGui::GetWindowSize().x * .5f) - ImGui::GetStyle().WindowPadding.x * 2 - ImGui::GetStyle().ItemSpacing.x) / 3, ImGui::GetWindowSize().y * .1f };
	if (ImGui::Button("Load", buttonSize)) {
		current = state::LOAD;
		selectedFile = -1;
	}

	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - (buttonSize.x * 3.0f) - (ImGui::GetStyle().ItemSpacing.x * 2.0f) - ImGui::GetStyle().WindowPadding.x);
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
		addedPrevious = false;
	}
	if (functionList.empty()) {
		ImGui::SetItemTooltip("Must create a function first.");
		ImGui::EndDisabled();
	}

	// add function button. we make sure that the first clause of the function exists and is zeroed out.
	ImGui::SameLine();
	if (ImGui::Button("Add Func", buttonSize)) {		
		current = FUNCTION;
		if (func->clause == nullptr) {
			std::vector<int>* temp = new std::vector<int>;
			temp->resize(func->attributeCount);
			std::fill(temp->begin(), temp->end(), 0);
			func->clause = temp;
		}
		functionIndex = functionList.size();
		if (std::count(functionList.begin(), functionList.end(), func) == 0) {
			functionList.insert(functionList.end(), func);
		}
		// resize the functions sub and sibling function lists so they dont waste memory
		func->subfunctionList.resize(func->attributeCount);
		func->siblingfunctionList.resize(func->targetAttributeCount - 1);
		for (int a = 0; a < func->targetAttributeCount-1; a++) {
			std::vector<std::vector<int>*>* temp = new std::vector<std::vector<int>*>;
			func->siblingfunctionList[a] = *temp;
		}
		addedPrevious = true;
	}

	// interview button
	ImGui::SameLine();
	if (ImGui::Button("Interview", buttonSize)) {	
		// go to the constraint screen
		//current = CONSTRAINT;
		current = PILOT;			// going straight to PILOT for now, since the constraints and pilot other questions are not implemented

		constraint.answers.resize(func->attributeCount);

		// set up some variables for the interview screen. like the function screen, we set a zeroed out vector for the function.
		std::vector<int>* temp = new std::vector<int>;
		temp->resize(func->attributeCount);
		std::fill(temp->begin(), temp->end(), 0);
		func->clause = temp;

		// make sure the function exists in the functionList, and not duplicated.
		if (std::count(functionList.begin(), functionList.end(), func) == 0) {
			functionList.insert(functionList.end(), func);
		}

		// for each sibling function, populate it with empty vectors.
		interview.datapoints.resize(func->siblingfunctionList.size());
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
		func->trueAttributes = new std::vector<int>(func->attributeCount);
		//
	}
	
	ImGui::End();
}

// draws the contraint screen, which lets the user set restraints on a given attribute of the function.
// Creates the cause and effect buttons, which are not working properly.
// Don't seem to have any effect on outputs anyways.
void Form::drawContraint () {
	ImGui::Begin("##con", &open, flags);
	ImGui::SetWindowSize(ImVec2{config::windowX * .75f, config::windowY * .5f});
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2{ (config::windowX - window.x) * .5f, (config::windowY - window.y) * .5f });

	// when the user presses a cause or effect, it sets a value in a vector.
	ImGui::PushFont(font);
	if (ImGui::BeginChild("##child", ImVec2{window.x - (ImGui::GetStyle().WindowPadding.x * 2), window.y * .8f})) {
		ImGui::SetNextWindowSize(ImVec2{ window.x * .5f, window.y * .5f });
		// sets a prerequisite for the attribute, the "cause".
		ImGui::BeginGroup();
		for (int a = 0; a < func->attributeCount; a++) {
			std::string t = func->attributeNames[a];
			if (a != 0) {
				t = "\n" + t;
			}
			ImGui::Text( t.c_str() );
			ImGui::Text("Cause");
			for (int b = 0; b < func->kValues[a]; b++) {
				// values set into the vector will be positive if they are a cause
				ImGui::RadioButton(("##aa" + std::to_string(a * func->attributeCount + b)).c_str(), &constraint.answers[a], b + 1);
				if (b != func->kValues[a] - 1) {
					ImGui::SameLine();
				}
			}
		}
		ImGui::EndGroup();
		//

		// sets an effect for the attribute.
		ImGui::SameLine(window.x * .5f);
		ImGui::SetNextWindowSize(ImVec2{ window.x * .5f, window.y * .5f });
		ImGui::BeginGroup();
		for (int a = 0; a < func->attributeCount; a++) {
			std::string t = "\n";
			if (a != 0) {
				t = "\n" + t;
			}
			ImGui::Text(t.c_str());
			ImGui::Text("Effect");
			for (int b = 0; b < func->kValues[a]; b++) {
				// values set into the vector will be negative if they are an effect.
				ImGui::RadioButton(("##bb" + std::to_string(a * func->attributeCount + b)).c_str(), &constraint.answers[a], -(b + 1));
				if (b != func->kValues[a] - 1) {
					ImGui::SameLine();
				}
			}
		}
		ImGui::EndGroup();
		ImGui::EndChild();
	}
	ImGui::PopFont();
	//
	
	// continue button
	ImGui::Separator();
	ImGui::SetCursorPos(ImVec2{window.x - (window.x * .2f + ImGui::GetStyle().WindowPadding.x), window.y - (ImGui::GetStyle().WindowPadding.y) - (window.y * .1f) });
	if (ImGui::Button("Continue", ImVec2{window.x * .2f, window.y * .1f}) ) {
		current = PILOT;
		std::cout << "Pressed" << std::endl;
	}
	ImGui::End();
}

// draws the load screen, which checks the models folder for .csv files. by default, when you press Finish on a function, it saves to this folder
void Form::drawLoad () {
	// preamble variables/settings
	ImGui::Begin("##", &open, flags | ImGuiWindowFlags_NoScrollbar);
	ImGui::SetWindowSize(ImVec2{config::windowX * .5f, config::windowY * .3f});
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2{ (config::windowX - window.x) * .5f, (config::windowY - window.y) * .5f} );

	// label
	ImVec2 textSize = ImGui::CalcTextSize("Load from which file?");
	ImGui::Text("Load from which file?");
	
	// files in folder
	ImGui::BeginChild("##fileNames", ImVec2{window.x - (ImGui::GetStyle().WindowPadding.x * 2.0f), window.y * .65f}, ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysVerticalScrollbar);
	for (int a = 0; a < files.size(); a++) {
		if (ImGui::Selectable(files[a].c_str(), selectedFile == a)) {
			selectedFile = a;
		}
	}
	ImGui::EndChild();

	// if we haven't selected a file, the load button should be disabled. 
	ImVec2 buttonSize{window.x * .2f, window.y * .3f - textSize.y - ImGui::GetStyle().WindowPadding.y * 2.0f - ImGui::GetStyle().ItemInnerSpacing.y * 2.0f };
	ImGui::SetCursorPosX(window.x - buttonSize.x - ImGui::GetStyle().WindowPadding.x);
	if (selectedFile == -1) {
		ImGui::BeginDisabled();
	}
	if (ImGui::Button("Load##", buttonSize)) {
		functionList.clear();
		CSVReader::readCSV(&functionList, (basePath + "\\" + files[selectedFile]).c_str());
		// once the file is read in, set the indexes to 0 across the board.
		func = functionList[0];
		current = state::FUNCTION;
		clauseIndex = 0;
		siblingFunctionIndex = 0;
		subfunctionIndex = 0;
		functionIndex = 0;
		if (func->siblingfunctionList[siblingFunctionIndex].size() != 0) {
			func->clause = func->siblingfunctionList[siblingFunctionIndex].at(0);
		}
	}
	if (selectedFile == -1) {
		ImGui::EndDisabled();
	}

	ImGui::End();
}

void start(moeka* edm, bool* flag) {
	(*edm).start(flag);
}

// draws a screen containing the pilot questions defined in a csv.
void Form::drawInterviewPilot () {
	
	// THIS STUFF IS NOT IMPLEMENTED FOR THE LOGIC, SO SKIP
	/*
	
	// TODO : add stuff for decision table for pilot questions
	ImGui::Begin("##", &open, flags | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	ImGui::SetWindowSize(ImVec2(config::windowX * .75f, config::windowY * .5f));
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2(window.x - (config::windowX * .625f), window.y - (config::windowY * .25f)));
	std::string question;

	// some setup variables for each question.
	ImGui::PushFont(font);
	std::string temp;
	bool isSlider = false;
	bool isTable = false;
	std::string tempString;
	for (int b = 0; b < config::pilotQuestions.size(); b++) {
		question = config::pilotQuestions[b][0];

		// on hovering (?) by a user, create a tooltip with a description of the question.
		ImGui::TextDisabled("(?)");
		if (ImGui::BeginItemTooltip()) {
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(config::pilotQuestionDescriptions[b].c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		// writes the question
		ImGui::SameLine();
		ImGui::PushTextWrapPos(window.x * .95);
		ImGui::TextWrapped(question.c_str());
		ImGui::PopTextWrapPos();

		ImGui::Separator();

		// if the question in the csv file contains a special token, then instead of radial buttons, we have a slider or table.
		if (interview.dt.questionType.contains(b)) {
			tempString = interview.dt.questionType.at(b);
			if (tempString.find("slider") != std::string::npos) {
				isSlider = true;
			}
			else if (tempString.find("table") != std::string::npos) {
				isTable = true;
			}
		}
		
		
		if (isSlider) {
			// if the slider tag was implemented correctly, the contents of the string stream should be
			// slider/min
			std::stringstream ss(tempString);
			std::string token;
			int min, max;
			std::getline(ss, token, '/');
			std::getline(ss, token, '/');
			min = stoi(token);
			if (interview.pilotAnswers[b] == 0) {
				interview.pilotAnswers[b] = min;
			}
			ImGui::SetNextItemWidth(window.x * .96);
			ImGui::SliderInt(std::string("##slider").append(std::to_string(b)).c_str(), &interview.pilotAnswers[b], min, func->targetAttributeCount);
		}
		else if (isTable) {
			// notice: when an element isTable, it currently only creates a table from ONE set of data, from the currently loaded function.
			ImVec2 tableSize {window.x * .96f, window.y * .5f };
			if (ImGui::BeginTable("##functiontableinterview", func->attributeCount, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollX | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_BordersOuterV, tableSize)) {
				for (int a = 0; a < func->attributeCount; a++) {
					ImGui::TableSetupColumn(func->attributeNames.at(a), 160);
				}
				ImGui::TableHeadersRow();
				for (int a = 0; a < func->attributeCount; a++) {
					ImGui::TableNextColumn();
					for (int b = 0; b < func->kValues.at(a); b++) {
						// TODO: read this data into somewhere else so its useable
						ImGui::RadioButton(std::to_string(b).append("##").append(std::to_string(a)).c_str(), &func->trueAttributes->data()[a], b);
					}
				}
				ImGui::EndTable();

				// need to make sure hardcoded index is always correct...
				interview.pilotAnswers[6] = 1;
			}
		}
		else {
			// if we don't have a special token, we render radial buttons instead.
			if (ImGui::BeginTable(std::string("##Question ").append(std::to_string(b)).c_str(), config::pilotQuestions[b].size() - 1, ImGuiTableFlags_ScrollX | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV, ImVec2{ window.x, window.y * .195f })) {
				for (int a = 1; a < config::pilotQuestions[b].size(); a++) {
					ImGui::TableNextColumn();
					temp = config::pilotQuestions[b][a];
					ImGui::RadioButton(temp.append("##").append(std::to_string(b)).c_str(), &interview.pilotAnswers[b], a);
				}
				ImGui::EndTable();
			}
		}
		isSlider = false;
		isTable = false;
	}
	ImGui::Separator();
	ImGui::PopFont();
	//


	// we must answer all questions before we move forward.
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


	// moves onto datapoint collection
	if (ImGui::Button("Next##", buttonSize)) {

	*/
		// TODO: take decision table and use it to initialize moeka object
		edm = new moeka('x');

		//std::vector<int> decisions = interview.dt.getDecision(interview.pilotAnswers);
		std::vector<int>decisions{};
		//decisions.resize(interview.pilotAnswers.size(), 0);
		decisions.resize(8, 0); // pilot questions are not properly being created, thus pilot answers aren't either. This stems from deicions table.
		std::vector<std::string> attrNames;

		for (auto a : func->attributeNames) {
			attrNames.push_back(a);
		}

		int functionKV = 2; 

		int staticInterChainOrder = 1; // 0 default, 1 SHCF, 2 LHCF, 3 LSO
		bool chainJump = -1;

		// TODO: user needs to select lowest acceptable datapoint from UI, then that is sent into moeka object init function
		bool majority = -1;

		int topBottomOrBinarySearch = -1;

		// TODO: implement this in decision table instead of hardcode? save last line of DT and getDecision should return above variables instead?
		// its just kind of weird to have DT when these if statements are needed anyway 

		// okay after implementing decision table seems useless for this application... 
		// would be easy to just take pilot answer data structure and iterate over it
		// DT is useful for presentatation and explanation though
		
		if (decisions[0] == 1) {
			staticInterChainOrder = 1; // SHCF
			topBottomOrBinarySearch = 1; // bottom
		}
		else if (decisions[1] == 1) {
			topBottomOrBinarySearch = 2; // binarySearch
		}
		else if (decisions[3] == 1) {
			staticInterChainOrder = 2; // LHCF
			topBottomOrBinarySearch = 0;
		}
		
		// if not using binary search and user is confident, use chainJump
		if (topBottomOrBinarySearch != 2 && decisions[2] == 1) {
			chainJump = true;
		}

		if (decisions[4] == 1) {
			staticInterChainOrder = 1; // SHCF only
		}

		if (decisions[5] == 1) {
			staticInterChainOrder == 2; // LHCF only
		}

		// needs to be 0 since majority is absence of answer
		if (decisions[6] == 0) {
			majority = true;
		}

		if (decisions[7] == 1) {
			staticInterChainOrder == 3; // LSO 
		}

		edm->initFromUI(func->attributeCount, attrNames, func->kValues, functionKV, staticInterChainOrder, *(func->trueAttributes), 
			chainJump, majority, topBottomOrBinarySearch);

		// now, need to start thread either in new functionor in drawInterview...
		std::thread thr(start, edm, startMoeka);
		thr.detach();

		current = INTERVIEW;
	//}

	// NO IDEA WHAT THIS COULD BE DOING.
	/*
	if (disable) {
		ImGui::EndDisabled();
	}
	*/
	//

	//ImGui::End();
}

// draws the interview screen, where we collect datapoint information from the user.
void Form::drawInterview () {
	// preamble variables/settings
	ImGui::Begin("##", &open, flags);
	ImGui::SetWindowSize(ImVec2(config::windowX * .75f, config::windowY * .085f * func->attributeCount));
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2(window.x - (config::windowX * .625f), config::windowY * .4f));
	ImGui::PushFont(font);

	dvector* datapoint = nullptr;

	bool end = false;

	while (true)
	{
		Sleep(100);
		// continue interview UI thread
		if (!*startMoeka && edm-> currentDatapoint->_class == -1)
		{
			datapoint = edm->currentDatapoint; // interview.datapoints[interview.hanselChainIndex][interview.datapointIndex];
			break;
		}
		// interview is done
		else if (!*startMoeka && edm->currentDatapoint->_class != -1)
		{
			action = state::PREP;
			open = !open;
			end = true;
			break;
		}
		
		/*
		// wait for background thread to fetch next interview question
		else
		{
			//std::cout << "UI: waiting for moeka thread..." << std::endl;
			Sleep(200);
		}
		*/
	}
	

	// window for getting the datapoint from the user
	if (!end)
	{
		ImGui::SetCursorPosX(window.x * .5f - ImGui::CalcTextSize("Input a class for this datapoint.").x * .5f);
		ImGui::Text("Input a target value (classification) for the datapoint.");
		ImGui::SetNextItemWidth(window.x * .2f);
		ImGui::Separator();
		ImGui::Text("Datapoint: ");
		for (int a = 0; a < datapoint->dataPoint.size(); a++) {
			ImGui::SetNextItemWidth(font->FontSize);

			// Print attribute name instead of just the numbers...
			ImGui::Text("%s: %d", func->attributeNames.at(a), datapoint->dataPoint.at(a));
		}
		
		// Retain currentClass across frames
		static int currentClass = -1;

		// using up and down to navigate the drop down menu
		if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
			currentClass = (currentClass - 1 <= -1) ? func->targetAttributeCount : currentClass - 1;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
			currentClass = (currentClass + 1 > func->targetAttributeCount) ? 0 : currentClass + 1;
		}

		// Update previewLabel before rendering the combo box
		static const char* previewLabel = (currentClass == -1) ? "Select Classification" : func->targetAttributeNames[currentClass];

		// Render the combo box
		if (ImGui::BeginCombo("##ClassDropdown", previewLabel)) {
			for (int i = 0; i <= func->targetAttributeCount; i++) {
				bool isSelected = (currentClass == i);

				const char* itemLabel = (i == func->targetAttributeCount) ? "N/A" : func->targetAttributeNames[i];

				if (ImGui::Selectable(itemLabel, isSelected)) {
					currentClass = (i == func->targetAttributeCount) ? -1 : i;
					previewLabel = (currentClass == -1) ? "N/A" : func->targetAttributeNames[currentClass];
				}

				if (isSelected) {
					ImGui::SetItemDefaultFocus(); // Focus on the selected item
				}
			}
			ImGui::EndCombo();
		}

		// Render the "Next" button and handle Enter key press
		ImGui::SetCursorPosY(window.y * 0.75f);
		ImVec2 buttonSize{ window.x * .2f, window.y * .2f };
		ImGui::SetCursorPosX(window.x * .56f - buttonSize.x);

		if (ImGui::Button("Next##", buttonSize) || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
			interview._class = currentClass;
			edm->currentClass = &interview._class;
			*startMoeka = true;
		}

		ImGui::PopFont();
		ImGui::SameLine();
		if (ImGui::Button("Done##", buttonSize)) {
			action = state::PREP;
			open = !open;
		}
	}
	// when the interview is done, as detetermined by the moeka thread
	else
	{
		ImGui::PopFont();

		// add model to model list
		//create hanselChainSet for function
		func->initializeHanselChains();

		// now that we are starting the hansel chains, we can copy over the information so that window can use it in displaying
		classCount = func->targetAttributeCount;
		classNames = func->targetAttributeNames;

		// TODO: move to hansel chain class?
		func->hanselChains = new HanselChains(func);
		func->hanselChains->attributes = func->kValues;
		func->hanselChains->dimension = func->attributeCount;
		//func->hanselChains->hanselChainContainsLowUnit.reserve(func->hanselChains->hanselChainSet.size());

		std::vector<std::vector<std::vector<int>>> temp(edm->hanselChainSet.size()); 

		// organize them and assign classes such that we can visualize
		for (const auto hc : edm->hanselChainSet)
		{
			temp.push_back({});

			for (auto e : hc) // copy by value
			{
				e.dataPoint.push_back(e._class);
				temp[temp.size() - 1].push_back(e.dataPoint);
			}
		}

		func->hanselChains->hanselChainSet = temp;

		// create a model for the hanselChains
		addModel = true;
		config::drawIndex++;
	}
	//

	ImGui::End();
}

// the form will not render if the window is not open.
void Form::openWindow () {
	open = true;
}

// draws the color window, where we can set the colors of datapoints.
void Form::drawColor () {
	// preamble variables/settings
	ImGui::Begin("Set the Colors of each Class Value##ClassColors", &open, flags);
	ImGui::SetWindowSize(ImVec2(config::windowX * .75f, config::windowY * .4f));
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2(config::windowX * .5f - window.x * .5f, config::windowY * .5f - window.y * .5f));

	// sets paddings and other variables for color squares.
	float paddingX = ImGui::GetStyle().WindowPadding.x * 2.0f;
	float itemSpacingX = ImGui::GetStyle().ItemSpacing.x;
	float size = (window.x - paddingX - (itemSpacingX * (config::classColors.size()-1)) ) / config::classColors.size();
	ImVec2 item{size, size};
	ImVec4 col;

	// check if the user wishes to use a gradient instead of setting each individual value.
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
		colorPickerState = func->targetAttributeCount;
		colorPickerOpen = true;
	}
	ImGui::PopStyleColor();
	ImGui::SeparatorText("Class Colors:");

	if (!useGradient) {
		ImGui::EndDisabled();
	}
	//

	// squares for setting individual class colors
	if (useGradient) {
		ImGui::BeginDisabled();
	}
	ImGui::PushFont(font);
	std::string base = "Class ";
	for (int a = 0; a < classCount; a++) {
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
	
	// opens a color picker for setting the color of a square (which sets the class color as well)
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
void Form::setNewFunc (std::string functionName) {
	char* name = new char[128];
	if (functionName.empty()) {
		strcpy_s(name, 128, std::string("Function ").append(std::to_string(functionList.size() + 1)).c_str());
	}
	else {
		// does not check if functionName can fit in buffer
		strcpy_s(name, 128, functionName.c_str());
	}
	
	func = new Function(name);
	func->kValues.resize(config::defaultAmount);
	func->attributeNames.resize(config::defaultAmount);
	func->targetAttributeNames.resize(config::defaultAmount); // this can also get resized just the same way, since we are always going to have at least two target attributes also
	// set default names of attributes
	int b = 0;
	for (auto a : func->attributeNames) {
		char* buff = new char[128];
		strcpy_s(buff, 128, std::string("Attribute ").append(std::to_string(b+1)).c_str() );
		func->attributeNames.at(b++) = buff;
	}
	b = 0;
	for (auto a : func->targetAttributeNames) {
		char* buff = new char[128];
		strcpy_s(buff, 128, std::string("Class ").append(std::to_string(b + 1)).c_str());
		func->targetAttributeNames.at(b++) = buff;
	}

	//
	std::fill(func->kValues.begin(), func->kValues.end(), 2);
	if (func->siblingfunctionList.empty()) {
		func->siblingfunctionList.push_back({});
	}

}