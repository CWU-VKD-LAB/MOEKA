#include "Section.h"

// set a default color for the section after we call Shape's constructor
Section::Section (): Shape (0, 0) {
	color = new ImVec4{ .5, .5, .5, 1 };
}

// destructor for the section. if we have managed children, or customized colors, delete them
Section::~Section () {
	for (auto a : managedList) {
		delete(a);
	}
	if (color != &config::defaultColor) {
		delete(color);
	}
}

// adds a drawable object to this objects managedList, then calculates what this objects compressed color should be.
void Section::addChild (Drawable* child) {
	managedList.push_back(child);
	// for x/y, we calculate the translation by using its current x/y, the stride x/y, half this objects width/height, and half the child drawables width/height.
	child->setTranslation(strideX + getX() - (getWidth() * 0.5f) + (child->getWidth() * 0.5f), strideY + getY() - (getHeight() * 0.5f) + (child->getHeight() * 0.5f));
	if (horizontal) {
		strideX += child->getWidth() + paddingX;
	}
	else {
		strideY += child->getHeight() + paddingY;
	}
	
	// average color for background
	color->x = 0.0f;
	color->y = 0.0f;
	color->z = 0.0f;
	color->w = 0.0f;
	float width = 0;
	float maxWidth = 0;
	float height = 0;
	float maxHeight = 0;
	Drawable* temp;

	for (int a = 0; a < managedList.size(); a++) {
		temp = managedList.at(a);
		color->x += temp->getR();
		color->y += temp->getG();
		color->z += temp->getB();
		color->w += temp->getA();

		if (a != 0) {
			width+=paddingX*totalScaleX;
			height+=paddingY*totalScaleY;
		}

		width += temp->getWidth();
		maxWidth = std::max(temp->getWidth(), maxWidth);
		height += temp->getHeight();
		maxHeight = std::max(temp->getHeight(), maxHeight);
	}

	color->x /= (float)managedList.size();
	color->y /= (float)managedList.size();
	color->z /= (float)managedList.size();
	color->w /= (float)managedList.size();

	// if this section lays out its children horizontally, scale to make sense horizontally, and visa versa
	if (horizontal) {
		resize(width, maxHeight);
	}
	else {
		resize(maxWidth, height);
	}
	
}

// changes where on the screen this object gets rendered, and the rendering positions of its children.
void Section::setTranslation (float dx, float dy) {
	// compute some variables then set the position matrix
	x = dx;
	y = dy;
	dx = 2 * (dx / config::windowX) - 1;
	dy = 2 * (dy / config::windowY) - 1;
	translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(dx, -dy, 0));
	shader->use();
	shader->setUniformMat4f("posMatrix", translateMatrix);

	// reset the stride then set the translations again for each child drawable.
	strideX = 0;
	strideY = 0;
	Drawable* temp;

	// for each child, set its position in this container.
	for (int a = 0; a < managedList.size(); a++) {
		temp = managedList.at(a);
		if (temp == nullptr) {
			continue;
		}
		// we calculate the translation by using its current x/y, the stride x/y, half this objects width/height, and half the child drawables width/height.
		temp->setTranslation(strideX + x - (getWidth() * 0.5f) + (temp->getWidth() * 0.5f), strideY + y - (getHeight() * 0.5f) + (temp->getHeight() * 0.5f));
		if (horizontal) {
			strideX += temp->getWidth()+(paddingX*totalScaleX);
		}
		else {
			strideY += temp->getHeight()+(paddingY*totalScaleY);

		}
	}
}

// sets the scale of this object, and the scale of its children.
void Section::setScale (float scaleX, float scaleY) {
	totalScaleX = scaleX;
	totalScaleY = scaleY;
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, 1));
	shader->use();
	shader->setUniformMat4f("scaleMatrix", scaleMatrix);
	scaleBounds(scaleX, scaleY);

	for (auto a : managedList) {
		a->setScale(scaleX, scaleY);
		compress = a->getWidth() <= config::compressValue;
	}
	setTranslation(getX(), getY());
}

void Section::setScaleX(float scale) {
	totalScaleX = scale;
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(totalScaleX, totalScaleY, 1));
	shader->use();
	shader->setUniformMat4f("scaleMatrix", scaleMatrix);
	scaleBounds(totalScaleX, totalScaleY);

	for (auto a : managedList) {
		a->setScale(totalScaleX, totalScaleY);
		compress = a->getWidth() <= config::compressValue;
	}
	setTranslation(getX(), getY());
}

void Section::setScaleY(float scale) {
	totalScaleY = scale;
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(totalScaleX, totalScaleY, 1));
	shader->use();
	shader->setUniformMat4f("scaleMatrix", scaleMatrix);
	scaleBounds(totalScaleX, totalScaleY);

	for (auto a : managedList) {
		a->setScale(totalScaleX, totalScaleY);
		compress = a->getWidth() <= config::compressValue;
	}
	setTranslation(getX(), getY());
}

// draws this object and its children.
void Section::draw () {
	// if we are compressing this section, then we need to make sure we have the average color of its children, and have
	// open GL draw that color in the shape of this section.
	if (compress) {
		color->x = 0;
		color->y = 0;
		color->w = 0;
		color->z = 0;
		for (auto a : managedList) {
			color->x += a->getR();
			color->y += a->getG();
			color->z += a->getB();
			color->w += a->getA();
		}
		color->x /= (float)managedList.size();
		color->y /= (float)managedList.size();
		color->z /= (float)managedList.size();
		color->w /= (float)managedList.size();
		bind();
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		return;
	}
	for (auto a : managedList) {
		a->draw();
	}
}

Drawable* Section::selected (GLFWwindow* window) {
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