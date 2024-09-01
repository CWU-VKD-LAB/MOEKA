#include "FileReader.h"

std::vector<Function*> CSVReader::loadedFunctionHistory{};

void CSVReader::readCSV(std::vector<Function*>* container, std::string path) {
	// TODO update to support child functions
	std::ifstream file(path);
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
			index = -1;
			char* name = new char[line.length()];
			strcpy_s(name, line.length(), line.substr(2, line.length()).c_str());
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


			tempFuncList.push_back(current);
			continue;
		}

		// everything below is either a clause or a subfunction.
		if (line.find('#') != std::string::npos) {
			std::vector<std::vector<int>*>* subfunction = new std::vector<std::vector<int>*>;
			current->subfunctionList.push_back(*subfunction);
			index++;
			continue;
		}

		std::vector<int>* clause = new std::vector<int>;
		current->subfunctionList[index].push_back(clause);

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
	// todo: update to support child functions
	std::ofstream file("output.csv");
	Function* f;

	// for each function in memory
	for (int a = 0; a < data->size(); a++) {
		f = data->at(a);
		// ## denotes to readFromCSV that this is a parent function.
		file << "##" << f->functionName << std::endl;

		// write down attribute names
		for (int b = 0; b < f->attributeCount; b++) {
			file << f->attributeNames[b] << ", ";
		}
		file << std::endl;

		// write down range of kValues
		for (int b = 0; b < f->attributeCount; b++) {
			file << f->kValues[b] << ", ";
		}
		file << std::endl;

		file << f->attributeCount << ", " << f->targetAttributeCount << ", " << std::endl;

		// for each subfunction of the original function
		for (int b = 0; b < f->subfunctionList.size(); b++) {
			file << "#sub" << b << std::endl;;
			for (int c = 0; c < f->subfunctionList[b].size(); c++) {
				for (int d = 0; d < f->subfunctionList[b][c]->size(); d++) {
					int temp = f->subfunctionList[b][c]->at(d);
					file << temp << ", ";
				}
				file << std::endl;
			}
		}
	}
	file.close();
}