#include "Model.h"

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

void Model::setTranslation(float x, float y) {
	// dont care. we don't set anything for the model. the model is just a container for a bunch of rectangles. 
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
