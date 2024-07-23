#ifndef SQUARE
#define SQUARE
#include <GL/glew.h>
#include "Shape.h"

class Square: public Shape{
public:
	// define a square given two 2d coordinates with an optional name.
	Square(float x1, float y1, float x2, float y2, std::string name = "Square");
	Square(float width, float height, std::string name = "Square");
	~Square();

	//
	static int count;
	std::string name;
};



#endif