#pragma once
#include <vector>

class Function {
public:
	int attributeCount = 2;
	std::vector<int> kValues{};
	std::vector<int>* clause = nullptr;
	std::vector<std::vector<int>*> clauseList{};

	~Function ();
};