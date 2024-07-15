#pragma once
#include "Shape.h"

class Drawable {
public:
	int positionX, positionY;
	float totalScale = 1;
	virtual void draw() {

	}
	virtual int getX () {return positionX;};
	virtual int getY () {return positionY;};
	virtual void setX(float value) {};
	virtual void setY(float value) {};
	virtual void setScale (float scale) {};
	virtual void setTranslation(float x, float y) {};
	virtual Shape* selectedShape() {return nullptr;};
};
