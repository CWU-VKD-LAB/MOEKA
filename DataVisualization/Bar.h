#pragma once
#include "Shape.h"

class Bar: public Shape {
private:
	int chainNumber = 0;
	ImVec4 individualColor {1.0f, 1.0f, 1.0f, 1.0f};
	
public:
	Bar (int classValue = 0, std::vector<int>* p = nullptr, float width = 10.0f, float height = 25.0f);
	int classVal = 0;
	std::vector<int> datapoint{};
	int inline getChainNumber () {return chainNumber;};
	void toggleIndividualColor () {
		useIndividualColor = !useIndividualColor;
	}
	bool useIndividualColor = false;
	void draw () override;
};
