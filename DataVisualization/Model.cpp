#include "Model.h"


void Model::addColumn (std::vector<int>* values) {
	Section* column = recursiveCreateSections(values);

	////int elements = (int)ceil((double)values.size() / config::compressBarAmount);
	//
	//// maybe make recursive method in disk to replace this?
	//for (int a = 0; a < values->size(); a++) {
	//	column->addChild(new Bar());
	//}
	// which side to set this to be drawn on
	int side = 0;
	// every other insertion should see side be negative
	if (list.size() != 0) {
		side = list.size() % 2 == 0 ? -1 : 1;
	}
	if (side == 1) {
		// every other insertion increases the stride
		stride += column->getWidth() + padding;
	}
	column->setTranslation(getX() + (stride * side), getY());
	list.push_back(column);
}

Section* Model::recursiveCreateSections (std::vector<int>* values) {
	if (values->size() <= config::compressBarAmount) {
		Section* s = new Section();
		for (auto a : *values) {
			s->addChild(new Bar(a));
		}
		return s;
	}

	size_t middle = values->size() / 2;
	std::vector<int> left {values->begin(), values->begin()+middle};
	std::vector<int> right {values->begin()+middle, values->end()};

	Section* s = new Section();

	s->addChild(recursiveCreateSections(&left));
	s->addChild(recursiveCreateSections(&right));

	return s;
}

Model::Model () {
	// the bounds of the model should be the entire screen.
	minBoundsX = -config::windowX;
	maxBoundsX = config::windowX;
	minBoundsY = -config::windowY;
	maxBoundsY = config::windowY;
}

Model::~Model () {
	for (auto a : list) {
		delete(a);
	}
}

// draw this object
void Model::draw () {
	for (auto a : list) {
		a->draw();
	}
}

Drawable* Model::selected (GLFWwindow* window) {
	// use BVH-esque techniques to find the hovered shape without checking too many shapes.
	double cx, cy;
	glfwGetCursorPos(window, &cx, &cy);


	// cx and cy will be in ranged of 0-600 from 0,0 on the top right to 600, 600, on the bottom right
	// check if cursor is in the bounds of this manager.
	double tempx = (double)getWidth() / 2.0f;
	double tempy = (double)getHeight() / 2.0f;
	if (getX() - tempx > cx || cx > getX() + tempx) {
		return nullptr;
	}
	if (getY() - tempy > cy || cy > getY() + tempy) {
		return nullptr;
	}
	// checks against a shapes bounding box.
	Drawable* d;
	for (auto s : list) {
		d = s->selected(window);
		if (d != nullptr) {
			return d;
		}
	}
	return nullptr;
}

void Model::setTranslation (float dx, float dy) {
	x = dx;
	y = dy;
	stride = 0;
	int pos = 0;
	for (auto a : list) {
		int side = 0;
		if (pos != 0) {
			side = pos % 2 == 0 ? -1 : 1;
		}
		if (side == 1) {
			stride += a->getWidth() + padding;
		}
		a->setTranslation(dx + stride * side, dy);
		pos++;
	}
}

void Model::setScale (float scale) {
	totalScale = scale;
	scaleBounds(scale);

	for (auto a : list) {
		a->setScale(scale);
	}

	setTranslation(getX(), getY());
}

// finds a scaling that will fit the model to the screen.
void Model::fitToScreen () {
	float largestWidth = 0.0f;
	float largestHeight = 0.0f;

	for (auto a : list) {
		largestWidth = std::max(largestWidth, a->getWidth());
		largestHeight = std::max(largestHeight, a->getHeight());
	}

	float ratioX = config::windowX / largestWidth;
	float ratioY = config::windowY / largestHeight;

	setScale(std::min(ratioX, ratioY));
	setTranslation(config::windowX / 2.0f, config::windowY / 2.0f);
}
