#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "../HanselChains.h"

class Function {
private:
	bool dnfOnlyVisualization = false;

public:
	int attributeCount = 2;
	int targetAttributeCount = 2;
	std::vector<int> kValues{};
	std::vector<char*> attributeNames{};

	std::vector<int>* clause = nullptr;
	std::vector<int>* trueAttributes = nullptr;
	std::vector<std::vector<std::vector<int>*>> subfunctionList{};
	
	char* functionName;

	HanselChains* hanselChains;

	/// @brief create the hansel chains
	void initializeHanselChains();

	/// @brief set them up for visualization if entering a function
	void setUpHanselChains();

	/// @brief set them up for interviewing
	void interviewHanselChains();

	void applyFunctiontoChains();

	Function (char* name);
	~Function ();
};