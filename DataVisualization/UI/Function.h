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
	int targetAttributeCount = 2;					// default of two, since we wouldn't ever be classifying things in 1 or less classes.
	std::vector<int> kValues{};
	std::vector<char*> attributeNames{};
	std::vector<char*> targetAttributeNames{};		// allows us to store names of each target class as well, useful if they're not just numerical. IE, leukemia, Cystic Fibrosis, Pneumonia...

	std::vector<int>* clause = nullptr;
	std::vector<int>* trueAttributes = nullptr;
	std::vector<std::vector<std::vector<int>*>> siblingfunctionList{};

	Function* parent = nullptr;
	std::vector<Function*> subfunctionList{};
	
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