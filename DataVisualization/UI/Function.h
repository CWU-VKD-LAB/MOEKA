#pragma once
#include <vector>

class Function {
public:
	int attributeCount = 1;
	std::vector<int> attributeValueCounts{};
	std::vector<int>* clause = nullptr;
	std::vector<std::vector<int>*> clauseList{};

	~Function ();
};