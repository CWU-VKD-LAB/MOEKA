#pragma once
#include "Shape.h"

class Bar: public Shape {
private:
	int chainNumber = 0;
public:
	Bar ();
	int inline getChainNumber () {return chainNumber;};
};
