#include "Form.h"

// draws the compare screen, where we can select two functions, test if they're comparable, then create a model that compares the two
// where the differences are shown as class values.
void Form::drawCompare() {
	ImGui::Begin("Compare Functions", &open, flags);
	ImVec2 window{ config::windowX * .75f, config::windowY * .175f };
	ImGui::SetWindowSize(window);
	ImGui::SetWindowPos(ImVec2((config::windowX - window.x) * .5f, (config::windowY - window.y) * .5f));
	ImGui::PushFont(font);

	char text[] = "Compare two Functions against each other by selecting them below";
	ImGui::SetCursorPosX((window.x - ImGui::CalcTextSize(text).x) * .5f);
	ImGui::Text(text);
	ImGui::PopFont();
	ImGui::Separator();


	// grab all function names
	std::vector<char*> names{};
	for (auto a : functionList) {
		names.push_back(a->functionName);
	}

	if (names.size() > 1) {
		// for each function name, add an option to the combo box.
		if (ImGui::BeginCombo("Function 1", names[compare.indexOne])) {
			for (int a = 0; a < names.size(); a++) {
				if (ImGui::Selectable(names[a], compare.indexOne)) {
					compare.indexOne = a;
					// make sure that compare.indexTwo does not have the same index as compare.indexOne
					if (compare.indexOne == compare.indexTwo) {
						compare.indexTwo = 0;
						while (compare.indexOne == compare.indexTwo) {
							compare.indexTwo++;
						}
					}
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Text("Compare to:");

		// for each function name, add an option to the combo box, skipping indexOne
		if (ImGui::BeginCombo("Function 2", names[compare.indexTwo])) {
			for (int a = 0; a < names.size(); a++) {
				if (a == compare.indexOne) {
					continue;
				}
				if (ImGui::Selectable(names[a], compare.indexTwo)) {
					compare.indexTwo = a;
				}
			}
			ImGui::EndCombo();
		}
	}
	else {
		ImGui::TextDisabled("There are not enough loaded functions to compare.");
	}

	// bottom row 
	ImGui::SetCursorPosY(window.y * 0.75f - ImGui::GetStyle().WindowPadding.y);
	ImGui::Separator();
	ImGui::SetCursorPosX(window.x * .7f - ImGui::GetStyle().WindowPadding.x);
	bool disabled = compare.indexOne == compare.indexTwo || functionList.size() < 2;
	if (disabled) {
		ImGui::BeginDisabled();
	}
	if (ImGui::Button("Compare", ImVec2{window.x * .3f, window.y * .23f})) {
		Function* funcOne = functionList[compare.indexOne];
		Function* funcTwo = functionList[compare.indexTwo];

		bool same = true;
		same = funcOne->attributeCount == funcTwo->attributeCount && same;
		same = funcOne->siblingfunctionList.size() == funcTwo->siblingfunctionList.size() && same;
		same = funcOne->subfunctionList.size() == funcTwo->subfunctionList.size() && same;

		if (same) {
			for (int a = 0; a < funcOne->kValues.size(); a++) {
				same = funcOne->kValues[a] == funcTwo->kValues[a] && same;
			}

			for (int b = 0; b < funcOne->targetAttributeCount - 1; b++) 
			{
				// TODO: bug here most likely b is too large for one func one or two
				same = (funcOne->siblingfunctionList[b].size() == funcTwo->siblingfunctionList[b].size()) && same;

				if (same) 
				{
					for (int c = 0; c < funcOne->siblingfunctionList[b].size(); c++) 
					{
						same = (funcOne->siblingfunctionList[b][c]->size() == funcTwo->siblingfunctionList[b][c]->size()) && same;
					}
				}
				
			}
		}
		if (same) {
			// free previous comparison function if it existed previously.
			if (comparisonFunction != nullptr) {
				delete(comparisonFunction);
			}

			// i believe we can directly insert into the model list instead of creating a function...
			std::vector<std::vector<int>>* comparisons = new std::vector<std::vector<int>>;

			for (int a = 0; a < funcOne->siblingfunctionList.size(); a++) 
			{
				//std::vector<std::vector<int>*>* sibling = new std::vector<std::vector<int>*>;

				for (int b = 0; b < funcOne->siblingfunctionList[a].size(); b++) 
				{
					std::vector<int> datapoint;

					for (int c = 0; c < funcOne->siblingfunctionList[a][b]->size(); c++) 
					{
						datapoint.push_back((1 - funcOne->siblingfunctionList[a][b]->at(c) == funcTwo->siblingfunctionList[a][b]->at(c)));
					}

					comparisons->push_back(datapoint);
				}

				//comparisonFunction->siblingfunctionList.push_back(*sibling);
			}

			// now add model directly to model list
			compare.comparisons = comparisons;

			// TODO remove commented out code?
			// 
			// create new comparison function
			/*
			char* name = new char[128];
			strcpy_s(name, 128, (funcOne->functionName + std::string(" Compare ") + funcTwo->functionName).c_str());
			comparisonFunction = new Function(name);

			for (auto a : funcOne->kValues) {
				comparisonFunction->kValues.push_back(a);
			}

			for (auto a : funcOne->attributeNames) {
				comparisonFunction->attributeNames.push_back(a);
			}

			comparisonFunction->attributeCount = funcOne->attributeCount;
			comparisonFunction->targetAttributeCount = funcOne->targetAttributeCount;
			
			

			for (int a = 0; a < funcOne->siblingfunctionList.size(); a++) {
				std::vector<std::vector<int>*>* sibling = new std::vector<std::vector<int>*>;
				for (int b = 0; b < funcOne->siblingfunctionList[a].size(); b++) {
					std::vector<int>* datapoint = new std::vector<int>;
					for (int c = 0; c < funcOne->siblingfunctionList[a][b]->size(); c++) {
						datapoint->push_back((1 - funcOne->siblingfunctionList[a][b]->at(c) == funcTwo->siblingfunctionList[a][b]->at(c)) );
					}
					sibling->push_back(datapoint);
				}
				comparisonFunction->siblingfunctionList.push_back(*sibling);
			}*/


			//comparisonFunction->clause = comparisonFunction->siblingfunctionList[0][0];
			
			
			//functionList.push_back(comparisonFunction);

			open = !open;

			////create hanselChainSet for function
			//result->initializeHanselChains();

			//// organize them and assign classes such that we can visualize
			//result->setUpHanselChains();

			// create a model for the hanselChains
			//addModel = true;
			//config::drawIndex++;

		}
	}
	if (disabled) {
		ImGui::EndDisabled();
	}

	ImGui::End();
}
