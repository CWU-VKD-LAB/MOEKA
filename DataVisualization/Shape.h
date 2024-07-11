#pragma once
//
#include <malloc.h>
#include <iostream>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <imgui.h>
//
#include "tools/config.h"
#include "tools/util.h"
#include "Render/IndexBuffer.h"
#include "Render/VertexBuffer.h"
#include "Render/VertexArray.h"
#include "Render/Shader.h"

class Shape {
public:
	VertexBuffer* vb;	// holds the point information
	IndexBuffer* ib;	// tells OpenGL how to draw the tri's
	VertexArray* va;	// tells OpenGL meta information on our shape
	Shader* shader;		// the program that tells OpenGL how to draw our shape
	//
	ImVec4 color{ 1, 1, 1, 1 };

	//
	std::vector<float> positions;
	std::vector<unsigned int> indices;

	//
	float minBoundsX = 1, minBoundsY = 1;
	float maxBoundsX = -1, maxBoundsY = -1;

	//
	int id;
	int kValue = -1;
	float x;
	float y;

	//
	void setup ();
	void draw ();
	void bind ();
	void setKValue (int val);
	int getKValue ();
	//
	void setPosition (float m_x, float m_y);
	float getPositionX ();
	float getPositionY ();

	~Shape ();
};
