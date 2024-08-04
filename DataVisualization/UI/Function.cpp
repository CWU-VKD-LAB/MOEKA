#include "Function.h"

Function::~Function () {
	delete(clause);
	for (auto a : clauseList) {
		delete(a);
	}
}
