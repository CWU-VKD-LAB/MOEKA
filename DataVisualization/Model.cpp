#include "Model.h"

Model::Model (GLFWwindow* m_window, float x, float y): window(m_window) {
	positionX = x;
	positionY = y;
	Window::addToRender(this);
};

Model::~Model () {
	// free the vector of disks.
	for (auto a : disks) {
		// free the individual disks.
		for (auto b : *a) {
			delete(b);
		}
		delete(a);
	}
}

void Model::draw () {
	for (auto a : disks) {
		for (auto b : *a) {
			b->draw();
		}
	}
}

void Model::createDisk (std::vector<int>& values) {
	std::vector<Disk*>* temp = new std::vector<Disk*>;

	int elements = (int)ceil((double)values.size() / config::compressBarAmount);
	int stride = 0;
	int valuesIndex = 0;

	for (int a = 0; a < elements; a++) {
		Disk* disk = new Disk{window, sizeX*config::compressBarAmount, sizeY, positionX, positionY + (sizeY+padding)*numOfDisks };
		
		// fill the disk segment
		for (int b = 0; b < config::compressBarAmount; b++) {
			// if OOB, stop adding squares.
			if (values.size() <= valuesIndex) {
				break;
			}

			Square* s = new Square(sizeX, sizeY);
			// set values here.
			s->setKValue(values[valuesIndex]);
			valuesIndex++;
			//

			disk->addShape(*s);
		}

		// set positions
		disk->positionX = positionX + stride;
		disk->positionY = positionY + ((sizeY + padding) * numOfDisks);

		// set transforms
		disk->setTranslation(positionX + stride, positionY + ((sizeY + padding) * numOfDisks));
		disk->setScale(1);

		// update variables
		stride += sizeX*config::compressBarAmount;
		temp->insert(temp->end(), disk);
	}
	numOfDisks++;
	disks.insert(disks.end(), temp);
}

// returns a bar in the model
Shape* Model::getBar(int disk, int index) {
	if (disk > disks.size()) {
		std::cout << "Invalid disk" << std::endl;
		return nullptr;
	}
	if (index > disks.at(disk)->size()*config::compressBarAmount) {
		std::cout << "Invalid index on disk" << std::endl;
		return nullptr;
	}
	return disks.at(disk)->at((int)floor(index / config::compressBarAmount))->managedList.at(index % config::compressBarAmount);
}

// returns the left coordinate of the models location
float Model::getX () {
	return positionX;
}

// returns the top coordinate of the models location
float Model::getY () {
	return positionY;
}

void Model::setX (float value) {
	positionX = value;
	float c = 0;
	for (auto a : disks) {
		for (auto b : *a) {
			b->setX(b->getX() + value + c);
			c += b->getSizeX();
		}
		c = 0;
	}
}

void Model::setY (float value) {
	positionY = value;
	float c = 0;
	for (auto a : disks) {
		for (auto b : *a) {
			b->setY(b->getY() + value + ((sizeY + padding) * c));
		}

		// heh
		c++;
	}
}

void Model::setScale (float scale) {
	totalScale = scale;
	for (auto a : disks) {
		for (auto b : *a) {
			b->setScale(scale);
		}
	}
	setTranslation(positionX, positionY);
}

void Model::setTranslation (float x, float y) {
	float c = 0;
	float d = 0;
	for (auto a : disks) {
		for (auto b : *a) {
			b->setTranslation(x + d, y + c);
			d += b->getSizeX();
		}
		d = 0;
		c += a->at(0)->getSizeY() + padding;
	}
}

Shape* Model::selectedShape() {
	Shape* s = nullptr;
	// model layer
	for (auto a : disks) {
		// disk layer
		for (auto b : *a) {
			s = b->selectedShape();
			if (s != nullptr) {
				return s;
			}
		}
	}
	return nullptr;
}


