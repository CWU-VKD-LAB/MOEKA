#pragma once
#include "Drawable.h"
#include <vector>
#include "Bar.h"
#include "Section.h"

class Model : public Drawable {
	std::vector<Section*> list{};
	float stride = 0;
	float padding = 2.0f;
public:
	Model();
	~Model();

	void addColumn (std::vector<int>* values);
	Section* recursiveCreateSections (std::vector<int>* values);
	void draw ();
	Drawable* selected (GLFWwindow* window);
	void setTranslation(float dx, float dy) override;
	void setScale(float scale) override;
	void fitToScreen();
};
