#pragma once
#include "Drawable.h"
#include <vector>
#include "Bar.h"
#include "../tools/globalVariables.h"

class Model : public Drawable {
	float stride = 0;
	float padding = 3.0f;
public:
	std::vector<Bar *> list{};
	Model();
	~Model();
	void draw ();
	Drawable* selected (GLFWwindow* window);
	void setTranslation(float dx, float dy) override;
	void setScale(float scaleX, float scaleY) override;
	void setScaleX(float scale) override;
	void setScaleY(float scale) override;
};
