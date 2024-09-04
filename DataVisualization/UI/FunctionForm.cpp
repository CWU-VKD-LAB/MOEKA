#include "Form.h"


// draws the function screen where the user can input clauses and save to file.
void Form::drawFunction() {
	ImGui::Begin("##", &open, flags);
	ImGui::SetWindowSize(ImVec2(config::windowX * .75f, config::windowY * .75f));
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2(window.x - (config::windowX * .625f), window.y - (config::windowY * .625f)));
	ImGui::SetCursorPosY(ImGui::GetStyle().WindowPadding.y);
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

	ImGui::Text((currentFunction + " " + std::to_string(functionIndex + 1) + "/" + std::to_string(functionList.size())).c_str());
	ImGui::SameLine();
	if (ImGui::ArrowButton("##lefta", ImGuiDir_Left)) {
		if (functionIndex > 0) {
			functionIndex--;
			func = functionList.at(functionIndex);
			subfunctionIndex = 0;
			clauseIndex = 0;
			action = "Add Clause";
		}
		func->clause = func->siblingfunctionList[subfunctionIndex].at(0);
	}
	ImGui::SameLine();
	if (ImGui::ArrowButton("##righta", ImGuiDir_Right)) {
		if (functionIndex < functionList.size() - 1) {
			functionIndex++;
			func = functionList.at(functionIndex);
			subfunctionIndex = 0;
			clauseIndex = 0;
			action = "Add Clause";
		}
		func->clause = func->siblingfunctionList[subfunctionIndex].at(0);
	}

	ImGui::Separator();

	// value setting field
	ImGui::BeginChild("##subwindow", ImVec2{ window.x - ImGui::GetStyle().WindowPadding.x * 2.0f, window.y * .63f }, ImGuiChildFlags_None);
	ImGui::SetNextItemWidth(100);
	if (ImGui::BeginTable("##functiontable", func->attributeCount, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollX | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_BordersOuterV)) {
		for (int a = 0; a < func->attributeCount; a++) {
			ImGui::TableSetupColumn(func->attributeNames.at(a), 160);
		}
		ImGui::TableHeadersRow();
		for (int a = 0; a < func->attributeCount; a++) {
			ImGui::TableNextColumn();
			for (int b = 0; b < func->kValues.at(a); b++) {
				if (func->clause != nullptr) {
					ImGui::RadioButton(std::to_string(b).append("##").append(std::to_string(a)).c_str(), &func->clause->data()[a], b);
				}

			}
		}
		ImGui::EndTable();
	}

	ImGui::EndChild();
	ImGui::PopFont();
	ImGui::Separator();
	//

	// clause list
	float test = ImGui::GetCursorPos().y; // for buttons later
	if (ImGui::BeginTable("##existing_clauses", (int)(func->siblingfunctionList[subfunctionIndex].size() + 1), ImGuiTableFlags_ScrollX | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV, ImVec2{ window.x * .67f, window.y * .13f })) {
		for (int a = 0; a < func->siblingfunctionList[subfunctionIndex].size(); a++) {
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(100);
			ImGui::Text(std::string("Clause ").append(std::to_string(a + 1)).c_str());
			ImGui::SameLine(85);

			bool disabled = a == clauseIndex;
			if (ImGui::Button(std::string("x##").append(std::to_string(a)).c_str(), ImVec2{ 15.0f, 15.0f })) {
				func->siblingfunctionList[subfunctionIndex].erase(func->siblingfunctionList[subfunctionIndex].begin() + a);
				statusMessage = std::string("Removed Clause ").append(std::to_string(a + 1));
				action = "Add Clause";
				if (disabled) {
					if (clauseIndex > func->siblingfunctionList[subfunctionIndex].size()) {
						clauseIndex--;
					}
					else {
						clauseIndex = 0;
					}
					func->clause = func->siblingfunctionList[subfunctionIndex].at(clauseIndex);
				}
			}
			// if we already have this clause loaded, disable the load button.
			if (disabled) {
				ImGui::BeginDisabled();
			}
			if (ImGui::Button(std::string("Load##").append(std::to_string(a)).c_str(), ImVec2{ 100, 38 })) {
				func->clause = func->siblingfunctionList[subfunctionIndex].at(a);
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


	// sub functions list
	if (ImGui::BeginTable("##subfunction", (int)(func->siblingfunctionList.size() + 1), ImGuiTableFlags_ScrollX | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV, ImVec2{ window.x * .67f, window.y * .13f })) {
		for (int a = 0; a < func->siblingfunctionList.size(); a++) {
			bool disabled = a == subfunctionIndex;
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(100);
			ImGui::Text(std::string("Sub-Func ").append(std::to_string(a + 1)).c_str());
			ImGui::SameLine(85);
			if (ImGui::Button(std::string("x##").append(std::to_string(a)).c_str(), ImVec2{ 15.0f, 15.0f })) {
				func->siblingfunctionList.erase(func->siblingfunctionList.begin() + a);
				statusMessage = std::string("Removed Sub ").append(std::to_string(a + 1));
				if (subfunctionIndex >= func->siblingfunctionList.size()) {
					subfunctionIndex--;
				}
			}
			if (disabled) {
				ImGui::BeginDisabled();
			}
			if (ImGui::Button(std::string("Load##").append(std::to_string(a)).c_str(), ImVec2{ 100, 38 })) {
				statusMessage = std::string("Loaded Sub ").append(std::to_string(a + 1));
				subfunctionIndex = a;
			}
			if (disabled) {
				ImGui::EndDisabled();
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
	if (ImGui::Button("Add a sibling function", buttonSize)) {
		std::vector<std::vector<int>*> subfunction;
		func->siblingfunctionList.insert(func->siblingfunctionList.end(), subfunction);
		subfunctionIndex++;
		statusMessage = "Added sibling function";
	}

	// new parent function
	ImGui::SetCursorPosX(xPos);
	yPos += buttonSize.y + stride;
	ImGui::SetCursorPosY(yPos);
	if (ImGui::Button("Add a sub function", buttonSize)) {
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

			if (func->siblingfunctionList.empty())
			{
				func->siblingfunctionList.push_back({});
			}
			func->siblingfunctionList[subfunctionIndex].insert(func->siblingfunctionList[subfunctionIndex].end(), func->clause);
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
		CSVReader::saveToCSV(&functionList, basePath + "\\" + functionList[0]->functionName + ".csv");
		open = !open;

		//create hanselChainSet for function
		func->initializeHanselChains();

		// organize them and assign classes such that we can visualize
		func->setUpHanselChains();

		// create a model for the hanselChains
		addModel = true;
		config::drawIndex++;

		files.clear();
		for (auto a : std::filesystem::directory_iterator(basePath)) {
			files.push_back(a.path().string().erase(0, basePath.length() + 1));
		}
	}

	//
	ImGui::End();
}
