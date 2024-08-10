#pragma once
#include <vector>
#include <string>

class Function {
public:
	int attributeCount = 2;
	std::vector<int> kValues{};
	std::vector<char*> attributeNames{};

	std::vector<int>* clause = nullptr;
	std::vector<std::vector<int>*> clauseList{};
	char* functionName;

	Function (char* name);
	~Function ();
};