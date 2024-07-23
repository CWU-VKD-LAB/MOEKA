#pragma once
#include "Shape.h"
#include <vector>
#include "tools/config.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Drawable.h"
#include "Square.h"

class ShapeManager: public Drawable {
protected:
	

	// restrict shapes in the bounds of this manager to what coordinates.
	float boundWidth, boundHeight;

	// draw the bounds of the manager
	bool drawBounds = true;
	Square* bounds;

	glm::mat4 transMatrix;
	glm::mat4 scaleMatrix;
	

	GLFWwindow* window = nullptr;
public:
	std::vector<Shape*> managedList{};
	ShapeManager(GLFWwindow* b_window, float bindWidth, float bindHeight, float posX, float posY);
	~ShapeManager();
	void setShapePosition(Shape& s, float x, float y, float scale = 1);

	virtual Shape* selectedShape() override;
	virtual void addShape (Shape& s);
	virtual void setTranslation (float x, float y) override;
	virtual void setScale (float scale) override;
	void setX (float value) override;
	void setY (float value) override;
	void draw () override;
	float inline getSizeX() {
		return (bounds->maxBoundsX - bounds->minBoundsX);
	}
	float inline getSizeY() {
		return (bounds->maxBoundsY - bounds->minBoundsY);
	}
};

