#pragma once
#include <imgui.h>
#include "glm/glm.hpp"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "tools/config.h"

class Drawable {
protected:
	float x = 0.0f, y = 0.0f;
	float totalScale = 1;
	ImVec4* color = &config::defaultColor;

	//
	float minBoundsX = 1, minBoundsY = 1;
	float maxBoundsX = -1, maxBoundsY = -1;
	//
	float originalMinBoundsX = 1, originalMinBoundsY = 1;
	float originalMaxBoundsX = -1, originalMaxBoundsY = -1;
	//
	glm::mat4 translateMatrix;
	glm::mat4 scaleMatrix;
public:
	virtual void draw () {};

	// getters for overall position/scale
	virtual float getX () {return x;};
	virtual float getY () {return y;};
	virtual float getScale () {return totalScale;};

	// transformations
	virtual void setScale (float scale) {};
	virtual void setTranslation (float dx, float dy) {};

	// getters/setters for attributes
	virtual float getWidth () {return maxBoundsX-minBoundsX;};
	virtual float getHeight () {return maxBoundsY-minBoundsY;};
	virtual void calculateBounds () {};
	virtual void scaleBounds (float value) {};
	virtual void setColor (float r, float g, float b, float a) {
		color->x = r;
		color->y = g;
		color->z = b;
		color->w = a;
		//color = {r, g, b, a};
	};

	// get color components
	float getR () {return color->x;};
	float getG () {return color->y;};
	float getB () {return color->z;};
	float getA () {return color->w;};
	ImVec4* getColor () {return color;};

	// method to find if the cursor lands in the bounds of this drawable
	virtual Drawable* selected(GLFWwindow* window) {
		double cx, cy;
		glfwGetCursorPos(window, &cx, &cy);

		// half width half height
		float hw = getWidth()/2.0f, hh = getHeight()/2.0f;
		
		if (cx < (double)x - hw || cx > (double)x + hw) {
			return nullptr;
		}
		if (cy < (double)y - hh || cy > (double)y + hh) {
			return nullptr;
		}
		return this;
	};
};
