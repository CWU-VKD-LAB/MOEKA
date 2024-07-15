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
	std::vector<Shape*> managedList{};

	// restrict shapes in the bounds of this manager to what coordinates.
	int boundWidth, boundHeight;

	// draw the bounds of the manager
	bool drawBounds = true;
	Square* bounds;

	glm::mat4 transMatrix;
	glm::mat4 scaleMatrix;
	

	GLFWwindow* window = nullptr;
public:
	ShapeManager(GLFWwindow* b_window, int bindWidth, int bindHeight, int posX, int posY);
	~ShapeManager();
	void setShapePosition(Shape& s, float x, float y, float scale = 1);

	virtual Shape* selectedShape() override;
	virtual void addShape (Shape& s);
	virtual void setTranslation (float x, float y) override;
	virtual void setScale(float scale) override;
	void setX(float value) override;
	void setY(float value) override;
	void draw() override;
};

