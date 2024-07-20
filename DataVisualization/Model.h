#pragma once
#include "Drawable.h"
#include "Disk.h"
#include <vector>
#include "Window.h"
#include "Shape.h"

class Model: public Drawable {
	std::vector<std::vector<Disk*>*> disks;
	GLFWwindow* window;
	int padding = 0;
	int numOfDisks = 0;
	int sizeX = 30;
	int sizeY = 100;
public:
	Model (GLFWwindow* m_window, int x, int y);
	~Model ();
	void draw () override;
	void createDisk (std::vector<int>& values);
	Shape* getBar (int disk, int index);

	int getX() override;
	int getY() override;
	void setX(float value) override;
	void setY(float value) override;
	void setScale(float scale) override;
	void setTranslation(float x, float y) override;
	Shape* selectedShape() override;
};