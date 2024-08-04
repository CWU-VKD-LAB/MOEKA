#pragma once
#include "Drawable.h"
#include "Shape.h"
#include <vector>

// This class is meant to be a container for Drawable objects. any actions done to this object will reflect on its children.
class Section: public Shape {
	std::vector<Drawable*> managedList {};

	// would probably be best to pack these pairs into a std::pair or a vector tbh
	float strideX = 0;
	float strideY = 0;
	float paddingX = 0;
	float paddingY = 2.0f;

	// if set to true, it will take the average color of children and display that instead of 
	// issuing draw calls for each child.
	bool compress = false;

	// if this object will display its children horizontally or not when drawn.
	bool horizontal = false;
public:
	Section();
	~Section ();
	void addChild (Drawable* child);
	void setTranslation (float dx, float dy) override;
	void setScale (float scale) override;
	void draw () override;
	Drawable* selected (GLFWwindow* window) override;
};
