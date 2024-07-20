#pragma once
#include "ShapeManager.h"

class Disk: public ShapeManager {
private:
	double stride;
	double padding = 0;
public:
	Disk(GLFWwindow* window, int bindWidth, int bindHeight, int posX, int posY);
	~Disk();
	void addShape (Shape& s) override;
	void setTranslation (float x, float y) override;
	void setScale (float scale) override;
	void draw () override;
};
