#include "Disk.h"

Disk::Disk (GLFWwindow* m_window, int bindWidth, int bindHeight, int posX, int posY)
	: ShapeManager(m_window, bindWidth, bindHeight, posX, posY) {
	stride = 0;
}

Disk::~Disk () {
	for (auto a : managedList) {
		delete(a);
	}
}

void Disk::addShape (Shape& s) {
	managedList.insert(managedList.end(), &s);

	// enforce layout
	s.shader->use();

	setShapePosition(s, positionX-(boundWidth/2) + stride, positionY-(boundHeight/2));
}


void Disk::setTranslation (float x, float y) {
	// set the relevant information 
	positionX = x;
	positionY = y;
	x = 2 * (x / config::windowX) - 1;
	y = 2 * (y / config::windowY) - 1;
	transMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, -y, 0));
	bounds->shader->use();
	bounds->shader->setUniformMat4f("posMatrix", transMatrix);

	// translate the shapes inside this manager appropriately to the new positioning/
	int t;
	stride = 0;
	for (auto a : managedList) {
		t = (a->maxBoundsX - a->minBoundsX);
		setShapePosition(*a, positionX - (boundWidth * totalScale / 2) + stride, positionY - (boundHeight * totalScale / 2));
		stride += t + padding;
	}
}

void Disk::setScale(float scale) {
	totalScale = scale;
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
	bounds->shader->use();
	bounds->shader->setUniformMat4f("scaleMatrix", scaleMatrix);
	for (auto a : managedList) {
		a->shader->use();
		a->shader->setUniformMat4f("scaleMatrix", scaleMatrix);
		a->scaleBounds(scale);
	}
	setTranslation(positionX, positionY);
}

