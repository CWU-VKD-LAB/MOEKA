#include "Model.h"

Model::Model (GLFWwindow* m_window, int x, int y): window(m_window), posX(x), posY(y) {};

Model::~Model () {
	// free the disks
	for (auto a : disks) {
		delete(a);
	}
}

void Model::draw () {
	for (auto a : disks) {
		if (a != nullptr) {
			a->draw();
		}
	}
}

void Model::createDisk(std::vector<int>& values) {
	int sizeX = 30;
	int sizeY = 50;
	Disk* disk = new Disk{window, (int)(sizeX*values.size()), sizeY, posX, posY+((sizeY+padding)*numOfDisks) };
	for (int a = 0; a < values.size()-1; a++) {
		Square* s = new Square{sizeX, sizeY};
		s->setKValue(values.at(a));
		disk->addShape(*s);
	}
	Window::managers.insert(Window::managers.end(), disk);
	std::cout << Window::managers.size() << std::endl;
	disks.insert(disks.end(), disk);
	numOfDisks++;
}

