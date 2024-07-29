#include "Disk.h"

Disk::Disk (): Shape (0, 0) {
	color = { .5, .5, .5, 1 };
}
Disk::~Disk() {
	for (auto a : managedList) {
		delete(a);
	}
}
void Disk::addChild(Drawable* child) {
	managedList.push_back(child);
	child->setTranslation(stride+getX()-(getWidth()/2.0f)+(child->getWidth()/2.0f), getY());
	stride += child->getWidth();
	
	// average color for later if compressed.
	color = {0, 0, 0, 0};
	float width = 0;
	float height = 0;
	for (auto a : managedList) {
		color.x += a->getR();
		color.y += a->getG();
		color.z += a->getB();
		color.w += a->getA();
		width += a->getWidth();
		height = std::max(getHeight(), child->getHeight());
	}
	color.x /= (float)managedList.size();
	color.y /= (float)managedList.size();
	color.z /= (float)managedList.size();
	color.w /= (float)managedList.size();

	resize(width, height);
}
void Disk::setTranslation(float dx, float dy) {
	// compute some variables then set the position matrix
	x = dx;
	y = dy;
	dx = 2 * (dx / config::windowX) - 1;
	dy = 2 * (dy / config::windowY) - 1;
	translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(dx, -dy, 0));
	shader->use();
	shader->setUniformMat4f("posMatrix", translateMatrix);

	// reset the stride then set the translations again for each child drawable.
	stride = 0;
	for (auto a : managedList) {
		if (a == nullptr) {
			continue;
		}
		a->setTranslation(x + stride + (a->getWidth() / 2.0f) - (getWidth() / 2.0f), y);
		stride += a->getWidth();
	}
}
void Disk::setScale(float scale) {
	totalScale = scale;
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
	shader->use();
	shader->setUniformMat4f("scaleMatrix", scaleMatrix);
	scaleBounds(scale);

	for (auto a : managedList) {
		a->setScale(scale);
		compress = a->getWidth() <= config::compressValue;
	}
	setTranslation(getX(), getY());
}
void Disk::draw() {
	if (compress) {
		for (auto a : managedList) {
			color.x += a->getR();
			color.y += a->getG();
			color.z += a->getB();
			color.w += a->getA();
		}
		color.x /= (float)managedList.size();
		color.y /= (float)managedList.size();
		color.z /= (float)managedList.size();
		color.w /= (float)managedList.size();
		bind();
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		return;
	}
	
	for (auto a : managedList) {
		a->draw();
	}
}

Drawable* Disk::selected (GLFWwindow* window) {
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
	for (auto s : managedList) {
		d = s->selected(window);
		if (d != nullptr) {
			return d;
		}
	}
	return nullptr;
}
