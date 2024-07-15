#include "Model.h"

Model::Model (GLFWwindow* m_window, int x, int y): window(m_window) {
	positionX = x;
	positionY = y;
	Window::addToRender(this);
};

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

void Model::createDisk (std::vector<int>& values) {
	Disk* disk = new Disk{window, (int)(sizeX*values.size()), sizeY, positionX, positionY+((sizeY+padding)*numOfDisks) };
	for (int a = 0; a < values.size()-1; a++) {
		Square* s = new Square{sizeX, sizeY};
		s->setKValue(values.at(a));
		disk->addShape(*s);
	}
	disk->positionX = positionX;
	disk->positionY = positionY + ((sizeY + padding) * numOfDisks);
	disk->setTranslation(positionX, positionY + ((sizeY + padding) * numOfDisks));
	disk->setScale(1);
	disks.insert(disks.end(), disk);
	numOfDisks++;
}

int Model::getX () {
	return positionX;
}

int Model::getY () {
	return positionY;
}

void Model::setX (float value) {
	positionX = value;
	for (auto a : disks) {
		a->setX(a->getX() + value);
	}
}

void Model::setY (float value) {
	positionY = value;
	int b = 0;
	for (auto a : disks) {
		std::cout << a->getY() + value + ((sizeY + padding) * b) << std::endl;
		a->setY(a->getY() + value + ((sizeY + padding)*b) );
		b++;
	}
}

void Model::setScale (float scale) {
	for (auto a : disks) {
		a->setScale(scale);
	}
}
void Model::setTranslation (float x, float y) {
	int b = 0;
	for (auto a : disks) {
		a->setTranslation(x, y + ((sizeY + padding) * b));
		b++;
	}
}

Shape* Model::selectedShape() {
	Shape* s = nullptr;
	for (auto a : disks) {
		s = a->selectedShape();
		if (s != nullptr) {
			return s;
		}
	}
	return nullptr;
}


