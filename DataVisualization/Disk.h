#pragma once
#include "Drawable.h"
#include "Shape.h"
#include <vector>

class Disk: public Shape {
	std::vector<Drawable*> managedList {};
	float stride = 0;
	float padding = 0;
	bool compress = false;
public:
	Disk();
	~Disk ();
	void addChild (Drawable* child);
	void setTranslation(float dx, float dy) override;
	void setScale(float scale) override;
	void draw() override;
	Drawable* selected (GLFWwindow* window) override;
};
