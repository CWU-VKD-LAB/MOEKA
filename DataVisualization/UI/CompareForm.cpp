#include "Form.h"

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

	ImGui::SetCursorPosY(window.y * 0.75f - ImGui::GetStyle().WindowPadding.y);
	ImGui::Separator();
	ImGui::SetCursorPosX(window.x * .7f - ImGui::GetStyle().WindowPadding.x);
	bool disabled = compare.indexOne != compare.indexTwo;
	if (disabled) {
		ImGui::BeginDisabled();
	}
	if (ImGui::Button("Compare", ImVec2{window.x * .3f, window.y * .23f})) {
		// some comparison done here.
	}
	if (disabled) {
		ImGui::EndDisabled();
	}

	ImGui::End();
}
