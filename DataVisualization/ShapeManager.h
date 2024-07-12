#pragma once
#include "Shape.h"
#include <vector>
#include "tools/config.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Drawable.h"
#include "Square.h"

class ShapeManager: Drawable {
protected:
	std::vector<Shape*> managedList{};
	int positionX, positionY;

	// restrict shapes in the bounds of this manager to what coordinates.
	int boundWidth, boundHeight;

	// draw the bounds of the manager
	bool drawBounds = true;
	Square* bounds;

	glm::mat4 transMatrix;
	glm::mat4 scaleMatrix;
	float totalScale = 1;

	GLFWwindow* window = nullptr;
public:
	ShapeManager(GLFWwindow* b_window, int bindWidth, int bindHeight, int posX, int posY);
	~ShapeManager();
	Shape* selectedShape ();

	virtual void addShape (Shape& s);
	virtual void setTranslation (float x, float y);
	virtual void setScale(float scale);
	int getX ();
	int getY ();
	void setX(int val);
	void setY(int val);
	void draw() override;
	void setShapePosition(Shape& s, float x, float y, float scale = 1);
};

