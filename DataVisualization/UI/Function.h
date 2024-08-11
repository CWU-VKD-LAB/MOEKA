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
	std::vector<int> kValues{};
	std::vector<char*> attributeNames{};

	std::vector<int>* clause = nullptr;
	std::vector<std::vector<std::vector<int>*>> subfunctionList{};
	
	char* functionName;

	HanselChains* hanselChains;

	void initializeHanselChains();

	void setUpHanselChains();

	void applyFunctiontoChains();

	Function (char* name);
	~Function ();
};