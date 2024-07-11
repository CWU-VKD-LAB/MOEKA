#pragma once
#include "Drawable.h"
#include "Disk.h"
#include <vector>
#include "Window.h"

class Model: Drawable {
	std::vector<Disk*> disks{};
	GLFWwindow* window;
	int posX, posY;
	int padding = 1;
	int numOfDisks = 0;
public:
	Model (GLFWwindow* m_window, int x, int y);
	~Model ();
	void draw () override;
	void createDisk (std::vector<int>& values);
};