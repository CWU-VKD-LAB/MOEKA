#include "Model.h"

// adds a column to the model in 5-3-1-2-4 order.
void Model::addColumn (std::vector<int>* values) {
	Section* column = recursiveCreateSections(values);

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

// recursively creates sections based on the length of the values list.
// if the size is greater than the compressBarAmount, then it will split the list.
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

// creates an empty model
Model::Model () {
	// the bounds of the model should be the entire screen.
	minBoundsX = -config::windowX;
	maxBoundsX = config::windowX;
	minBoundsY = -config::windowY;
	maxBoundsY = config::windowY;
}

// frees up the sections this model holds.
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

// registers a mouse click and determines what bar (if any) was selected by the click. 
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

// sets the translation of the model and all its children, enforcing formatting.
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

// set the scaleX and scaleY of the model, with a translation to its current position to update the childrens positions with the size change.
void Model::setScale (float scaleX, float scaleY) {
	totalScaleX = scaleX;
	totalScaleY = scaleY;
	scaleBounds(totalScaleX, totalScaleY);

	for (auto a : list) {
		a->setScale(scaleX, scaleY);
	}
	setTranslation(getX(), getY());
}

// sets the scaleX of the model
void Model::setScaleX (float scale) {
	totalScaleX = scale;
	scaleBounds(totalScaleX, totalScaleY);

	for (auto a : list) {
		a->setScale(totalScaleX, totalScaleY);
	}
	setTranslation(getX(), getY());
}

// sets the scaleY of the model
void Model::setScaleY(float scale) {
	totalScaleY = scale;
	scaleBounds(totalScaleX, totalScaleY);

	for (auto a : list) {
		a->setScale(totalScaleX, totalScaleY);
	}
	setTranslation(getX(), getY());
}

// finds a scaling that will fit the model to the screen.
void Model::fitToScreen () {
	float width = 0.0f;
	float largestHeight = 0.0f;

	// calculate the size of the column
	for (auto a : list) {
		float totalHeight = 0;
		float w = 0;
		for (auto b : a->managedList) {
			totalHeight += b->getHeight();
			w = std::max(b->getWidth(), w);
		}
		width += w;
		largestHeight = std::max(largestHeight, totalHeight);
	}

	float ratioX = config::windowX / width;
	float ratioY = config::windowY / largestHeight;

	// set the scale based on the calculated ratios and set the translation to the center of the screen.
	setScale(ratioX, ratioY);
	setTranslation(config::windowX / 2.0f, config::windowY / 2.0f);
	totalScaleX = 1;
	totalScaleY = 1;
}
