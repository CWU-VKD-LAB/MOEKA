#include "Disk.h"

Disk::Disk(GLFWwindow* m_window, int bindWidth, int bindHeight, int posX, int posY)
	: ShapeManager(m_window, bindWidth, bindHeight, posX, posY) {

	stride = 0;
}

void Disk::addShape(Shape& s) {
	managedList.insert(managedList.end(), &s);

	// enforce layout
	s.shader->use();
	s.shader->setUniformMat4f("posMatrix", transMat);

	// translate based on the stride
	int t = (s.maxBoundsX - s.minBoundsX);
	setShapePosition(s, positionX-(boundWidth/2) + stride, positionY-(boundHeight/2));
	stride += t;
}


void Disk::setTranslation(float x, float y) {
	// set the relevant information 
	positionX = x;
	positionY = y;
	x = 2 * (x / config::windowX) - 1;
	y = 2 * (y / config::windowY) - 1;
	transMat = glm::translate(glm::mat4(1.0f), glm::vec3(x, -y, 0)) * config::proj;
	bounds->shader->use();
	bounds->shader->setUniformMat4f("posMatrix", transMat);

	// translate the shapes inside this manager appropriately to the new positioning/
	int t;
	stride = 0;
	for (auto a : managedList) {
		t = (a->maxBoundsX - a->minBoundsX);
		setShapePosition(*a, positionX - (boundWidth / 2) + stride, positionY - (boundHeight / 2));
		stride += t;
	}
}

