#pragma once
#include "Drawable.h"
#include <vector>
#include "Bar.h"
#include "Disk.h"

class Model : public Drawable {
	std::vector<Disk*> list{};
	float stride = 0;
public:
	Model();
	~Model();

	void addDisk (std::vector<int> values);
	void draw ();
	Drawable* selected (GLFWwindow* window);
	void setTranslation(float dx, float dy) override;
	void setScale(float scale) override;
	void fitToScreen();
};
