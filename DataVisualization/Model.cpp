#include "Model.h"

// adds a column to the model
void Model::addColumn(std::vector<int>* values, std::vector<std::vector<int>>* points) {

	// Create a new Section
	Section* s = new Section();

	// add a new bar with it's class and corresponding data point
	for (int i = 0; i < values->size(); i++) {
		s->addChild(new Bar(values->at(i), &points->at(i)));
	}

	// Add the column to the list.
	list.push_back(s);
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

// draw this model by drawing all the sections we have stored in the list. thus this is function calling the section draw function on everyone in the list.
void Model::draw() {
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
void Model::setTranslation(float dx, float dy) {
	x = dx;
	y = dy;

	float currentX = dx;  // Start from the base x position on the left side of the screen.
	for (auto a : list) {
		// calls the SECTION setTranslation function
		a->Section::setTranslation(currentX, dy);
		currentX += a->getWidth() + padding;  // Increment by the width and padding
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
	//setTranslation(getX(), getY());
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
	float width = 0.0f;
	float largestHeight = 0.0f;
   // Iterate through the list of sections (list) and calculate the total height and maximum width. (They should all be the same width though...)
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
	
	setTranslation(config::windowX / 8.0f, (config::windowY / 2.0f));
	totalScaleX = 1;
	totalScaleY = 1;
}
