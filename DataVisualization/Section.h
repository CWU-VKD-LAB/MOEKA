#pragma once
#include "Drawable.h"
#include "Shape.h"
#include <vector>

// This class is meant to be a container for Drawable objects. any actions done to this object will reflect on its children.
class Section: public Shape {

	// would probably be best to pack these pairs into a std::pair or a vector tbh
	float strideX  = 0.0f;
	float strideY  = 0.0f;
	float paddingX = 3.0f;
	float paddingY = 3.0f;

	// if this object will display its children horizontally or not when drawn.
	bool horizontal = false;
public:
	std::vector<Drawable*> managedList{};
	Section();
	~Section ();
	void addChild (Drawable* child);
	void setTranslation (float dx, float dy) override;
	void setScale (float scaleX, float scaleY) override;
	void setScaleX (float scale) override;
	void setScaleY (float scale) override;
	void draw () override;
	Drawable* selected (GLFWwindow* window) override;
};
