#include "FileReader.h"

std::vector<Function*> CSVReader::loadedFunctionHistory{};

void CSVReader::readCSV(std::vector<Function*>* container, std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "Failed to open file" << std::endl;
		return;
	}
	std::string line;
	std::string delimiter = ", ";
	std::string token;
	std::vector<Function*> tempFuncList;
	Function* current = nullptr;
	int index = -1;

	while (std::getline(file, line)) {
		// if there is for some reason a blank line, skip.
		if (line.empty()) {
			continue;
		}

		// create a function if we have ##
		if (line.find("##") != std::string::npos) {
			int depth = (int)std::count(line.begin(), line.end(), '#') - 2;

			index = -1;
			char* name = new char[line.length()];
			strcpy_s(name, line.length(), line.substr(2+depth, line.length()).c_str());
			current = new Function(name);

			// read attribute info
			std::getline(file, line);
			std::stringstream ss(line);
			std::getline(file, line);
			std::stringstream kValueSS(line);
			std::string attributeName;
			std::string kVal;
			while (!ss.eof()) {
				// deal with attribute names
				std::getline(ss, attributeName, ',');
				if (attributeName[0] == ' ') {
					attributeName = attributeName.substr(1, attributeName.length());
				}
				if (!attributeName.empty()) {
					char* attribName = new char[attributeName.length() + 1];
					strcpy_s(attribName, attributeName.size() + 1, attributeName.c_str());
					current->attributeNames.push_back(attribName);
				}

				// deal with kValues
				std::getline(kValueSS, kVal, ',');
				if (kVal[0] == ' ') {
					kVal = kVal.substr(1, kVal.length());
				}
				if (!kVal.empty()) {
					current->kValues.push_back(stoi(kVal));
				}
			}
			// include attribute names in case the user slides for more of them.
			std::string temp;
			while (current->attributeNames.size() != config::defaultAmount) {
				temp = "Attribute " + std::to_string(current->attributeNames.size() + 1);
				char* n = new char[temp.length() + 1];
				strcpy_s(n, temp.size() + 1, temp.c_str());
				current->attributeNames.push_back(n);
				current->kValues.push_back(2);
			}
			//// attribute count and target attribute count
			std::getline(file, line);
			current->attributeCount = stoi(line.substr(0, line.find(", ")));
			line.erase(0, line.find(", ") + 2);
			current->targetAttributeCount = stoi(line.substr(0, line.length() - 1));

			if (depth != 0) {
				Function* parent = tempFuncList.back();
				while (depth > 1) {
					parent = parent->subfunctionList.back();
					depth--;
				}
				parent->subfunctionList.push_back(current);
				current->parent = parent;
			}
			else {
				tempFuncList.push_back(current);
			}
			continue;
		}

		// everything below is either a clause or a siblingfunction.
		if (line.find('#') != std::string::npos) {
			std::vector<std::vector<int>*>* subfunction = new std::vector<std::vector<int>*>;
			current->siblingfunctionList.push_back(*subfunction);
			index++;
			continue;
		}

		std::vector<int>* clause = new std::vector<int>;
		current->siblingfunctionList[index].push_back(clause);

		std::stringstream ss(line);
		std::string token;
		while (!ss.eof()) {
			std::getline(ss, token, ',');
			if (token[0] == ' ') {
				token = token.substr(1, token.length());
			}
			if (!token.empty()) {
				clause->push_back(stoi(token));
			}
		}
	}

	file.close();
	container->clear();
	for (auto a : tempFuncList) {
		loadedFunctionHistory.push_back(a);
		container->push_back(a);
	}
}

// saves the contents of the functionList and its related clauses to a CSV file.
void CSVReader::saveToCSV(std::vector<Function*>* data, std::string path) {
	std::ofstream file(path);

	// for each function in memory
	for (int a = 0; a < data->size(); a++) {
		saveFunction(file, data->at(a), 0);
	}
	file.close();
}

void CSVReader::saveFunction (std::ofstream &file, Function* func, int depth) {
	for (int a = 0; a < depth+2; a++) {
		file << "#";
	}
	file << func->functionName << std::endl;

	// write down attribute names
	for (int b = 0; b < func->attributeCount; b++) {
		file << func->attributeNames[b] << ", ";
	}
	file << std::endl;

	// write down range of kValues
	for (int b = 0; b < func->attributeCount; b++) {
		file << func->kValues[b] << ", ";
	}
	file << std::endl;

	file << func->attributeCount << ", " << func->targetAttributeCount << ", " << std::endl;

	// for each subfunction of the original function
	for (int b = 0; b < func->siblingfunctionList.size(); b++) {
		file << "#sib" << b << std::endl;;
		for (int c = 0; c < func->siblingfunctionList[b].size(); c++) {
			for (int d = 0; d < func->siblingfunctionList[b][c]->size(); d++) {
				int temp = func->siblingfunctionList[b][c]->at(d);
				file << temp << ", ";
			}
			file << std::endl;
		}
	}

	for (auto a : func->subfunctionList) {
		if (a)
		{
			saveFunction(file, a, depth + 1);
		}
	}
}
