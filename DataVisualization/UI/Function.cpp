#include "Function.h"

Function::Function (char* name) {
	functionName = name;
}

Function::~Function () {
	delete(clause);
	for (auto a : clauseList) {
		delete(a);
	}
	for (auto a : attributeNames) {
		delete(a);
	}
	delete(functionName);
}
