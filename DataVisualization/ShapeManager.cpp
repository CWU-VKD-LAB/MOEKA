#include "ShapeManager.h"


ShapeManager::ShapeManager (GLFWwindow* m_window, int bindWidth, int bindHeight, int posX, int posY): boundHeight(bindHeight), boundWidth(bindWidth), positionX(posX), positionY(posY), window(m_window) {
	bounds = new Square{bindWidth, bindHeight, "Manager"};
	bounds->shader->use();
	bounds->color.x = .5*(posY/600);
	bounds->color.y = .5;
	bounds->color.z = .5;
	bounds->color.w = 1;

	setShapePosition(*bounds, positionX, positionY);
	setScale(totalScale);
	
}


ShapeManager::~ShapeManager () {
	delete(bounds);
}
Shape* ShapeManager::selectedShape () {
	// use BVH-esque techniques to find the hovered shape without checking too many shapes.
	double cx, cy;
	glfwGetCursorPos(window, &cx, &cy);
	
	// cx and cy will be in ranged of 0-600 from 0,0 on the top right to 600, 600, on the bottom right
	// check if cursor is in the bounds of this manager.
	double tempx = boundWidth/2 * totalScale;
	double tempy = boundHeight/2 * totalScale;
	if (positionX - tempx > cx || cx > positionX + tempx) {
		return nullptr;
	}
	if (positionY - tempy > cy || cy > positionY + tempy) {
		return nullptr;
	}
	
	// checks against a shapes bounding box.
	double width, height;
	for (auto s : managedList) {
		width = (s->maxBoundsX - s->minBoundsX)/2;
		height = (s->maxBoundsY - s->minBoundsY)/2;

		if (cx < s->x - width || cx > s->x + width) {
			continue;
		}
		if (cy < s->y - height || cy > s->y + height) {
			continue;
		}
		return s;
	}

	return nullptr;
}


void ShapeManager::setShapePosition (Shape& s, float x, float y, float scale) {
	// check if x or y go beyond the manager bounds.
	// TODO

	// scale the bounds of the shape before we actually scale it.


	// set the shapes position variables incase we need to reference later
	x += (s.maxBoundsX-s.minBoundsX)/2;
	y += (s.maxBoundsY-s.minBoundsY)/2;
	s.setPosition(x, y);

	// convert the coordinates to its relative to the center of the manager
	x = 2 * (x / config::windowX) - 1;
	y = 2 * (y / config::windowY) - 1;

	// apply translation
	s.shader->use();
	glm::mat4 temp = glm::translate(glm::mat4(1.0f), glm::vec3(x, -y, 0));
	s.shader->setUniformMat4f("posMatrix", temp);
	s.shader->setUniformMat4f("projMatrix", config::proj);
	s.shader->setUniformMat4f("scaleMatrix", scaleMatrix);
}



void ShapeManager::addShape (Shape& s) {
	// add a shape to the managed list, then set its translation to the managers. by default it will show in the center of the manager.
	managedList.insert(managedList.end(), &s);
	// add half the shapes width and height so the 0,0 position is the top left corner rather than the middle.
	setShapePosition(s, (s.maxBoundsX - s.minBoundsX)/2, (s.maxBoundsY - s.minBoundsY) / 2);
	
}



void ShapeManager::draw () {
	if (drawBounds) {
		bounds->draw();
	}
	for (auto a : managedList) {
		a->draw();
	}
}



void ShapeManager::setTranslation (float x, float y) {
	positionX = x;
	positionY = y;
	x = 2 * (x / config::windowX) - 1;
	y = 2 * (y / config::windowY) - 1;
	transMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, -y, 0));
	bounds->shader->use();
	bounds->shader->setUniformMat4f("posMatrix", transMatrix);
	for (auto a : managedList) {
		a->shader->use();
		a->shader->setUniformMat4f("posMatrix", transMatrix);
	}
}

void ShapeManager::setScale (float scale) {
	totalScale = scale;
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
	bounds->shader->use();
	bounds->shader->setUniformMat4f("scaleMatrix", scaleMatrix);
	for (auto a : managedList) {
		a->shader->use();
		a->shader->setUniformMat4f("scaleMatrix", scaleMatrix);
		a->scaleBounds(scale);
	}
}

int ShapeManager::getX () {
	return positionX;
}
int ShapeManager::getY () {
	return positionY;
}
void ShapeManager::setX(int val) {
	positionX = val;
}
void ShapeManager::setY(int val) {
	positionY = val;
}
