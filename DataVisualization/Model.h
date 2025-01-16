#pragma once
#include "Drawable.h"
#include <vector>
#include "Bar.h"
#include "Section.h"

class Model : public Drawable {
	std::vector<Section*> list{};
	float stride = 0;
	float padding = 1.0f;
public:
	Model();
	~Model();
	void addColumn (std::vector<int>* values, std::vector<std::vector<int>>* points);
	Section* createSections (std::vector<int>* values, std::vector<std::vector<int>>* points);
	void draw ();
	Drawable* selected (GLFWwindow* window);
	void setTranslation(float dx, float dy) override;
	void setScale(float scaleX, float scaleY) override;
	void setScaleX(float scale) override;
	void setScaleY(float scale) override;
	void fitToScreen();
};
