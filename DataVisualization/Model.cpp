#include "Model.h"

// adds a column to the model
void Model::addColumn(std::vector<int>* values) {
	Section* column = createSections(values);

	// Set the translation directly without modifying stride
	column->setTranslation(getX(), getY());

	// Add the column to the list
	list.push_back(column);
}

// recursively creates sections based on the length of the values list.
// if the size is greater than the compressBarAmount, then it will split the list.
Section* Model::createSections(std::vector<int>* values) {
	// Create a new Section
	Section* s = new Section();

	// Add a Bar for each value in the vector
	for (auto a : *values) {
		s->addChild(new Bar(a));
	}
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

void Model::fitToScreen() {
	// Calculate the total width and height of the model based on the bars' sizes.
	float totalWidth = 0.0f;
	float totalHeight = 0.0f;

	// Iterate through the list of sections (list) and calculate the total width and maximum height.
	for (auto a : list) {
		float sectionWidth = 0.0f;
		float sectionHeight = 0.0f;

		for (auto b : a->managedList) {
			sectionWidth += b->getWidth();
			sectionHeight = std::max(sectionHeight, b->getHeight());
		}

		totalWidth += sectionWidth;
		totalHeight = std::max(totalHeight, sectionHeight);
	}

	// Scale the model to exactly fit the window
	float scaleX = std::min(1.0f, config::windowX / totalWidth);
	float scaleY = std::min(1.0f, config::windowY / totalHeight);

	// Set the scale for the model to fit the window size.
	setScale(scaleX, scaleY);

	// After scaling, center the model in the window.
	setTranslation(config::windowX / 2.0f, config::windowY / 2.0f);
}

