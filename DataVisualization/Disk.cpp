#include "Disk.h"

Disk::Disk (GLFWwindow* m_window, int bindWidth, int bindHeight, float posX, float posY)
	: ShapeManager(m_window, bindWidth, bindHeight, posX, posY) {
	stride = 0;
}

Disk::~Disk () {
	for (auto a : managedList) {
		delete(a);
	}
}

void Disk::draw () {
	// if no shapes inside just draw the bounds
	if (managedList.empty()) {
		bounds->draw();
		return;
	}
	// if there is shapes inside, if a given shape has horizontal size larger than 1
	// then its probably safe to draw them without worry.
	if (managedList.at(0)->getSizeX() > 1) {
		for (auto a : managedList) {
			a->draw();
		}
		return;
	}
	// if the shape was smaller in size than 1 pixel then we draw the bounds,
	// but we change the color of the bounds first by averaging its children.
	std::cout << "Too small! Compressing..." << std::endl;
	ImVec4 color{ 0, 0, 0, 0 };
	for (auto a : managedList) {
		color.x += a->color.x;
		color.y += a->color.y;
		color.z += a->color.z;
		color.w += a->color.w;
	}
	color.x /= managedList.size();
	color.y /= managedList.size();
	color.z /= managedList.size();
	color.w /= managedList.size();
	bounds->color = color;
	bounds->draw();
}

void Disk::addShape (Shape& s) {
	managedList.insert(managedList.end(), &s);

	// enforce layout
	setShapePosition(s, (float)(positionX-(boundWidth/2) + stride), (float)(positionY-(boundHeight/2)));
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
	double t;
	stride = 0;
	for (auto a : managedList) {
		t = ((double)a->maxBoundsX - a->minBoundsX);
		setShapePosition(*a, (float)(positionX - ((double)boundWidth * totalScale / 2.0) + stride), (float)(positionY - ((double)boundHeight * totalScale / 2.0)));
		stride += t + padding;
	}
}

void Disk::setScale(float scale) {
	totalScale = scale;
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
	bounds->shader->use();
	bounds->shader->setUniformMat4f("scaleMatrix", scaleMatrix);
	bounds->scaleBounds(scale);
	for (auto a : managedList) {
		a->scaleBounds(scale);
		a->shader->use();
		a->shader->setUniformMat4f("scaleMatrix", scaleMatrix);
	}
	setTranslation(positionX, positionY);
}

