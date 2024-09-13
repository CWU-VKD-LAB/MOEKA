#pragma once
#include "Shape.h"

class Bar: public Shape {
private:
	int chainNumber = 0;
public:
	Bar (int classValue = 0);
	int classVal = 0;
	int inline getChainNumber () {return chainNumber;};
};
