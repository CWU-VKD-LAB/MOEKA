#pragma once
#include "Shape.h"

class Drawable {
public:
	float positionX = 0.0, positionY = 0.0;
	float totalScale = 1;
	virtual void draw() {

	}
	virtual float getX () {return positionX;};
	virtual float getY () {return positionY;};
	virtual void setX(float value) {};
	virtual void setY(float value) {};
	virtual void setScale (float scale) {};
	virtual void setTranslation(float x, float y) {};
	virtual Shape* selectedShape() {return nullptr;};

};
