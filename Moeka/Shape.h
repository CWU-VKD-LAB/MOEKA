#pragma once
#include "Drawable.h"
#include "Render/IndexBuffer.h"
#include "Render/VertexBuffer.h"
#include "Render/VertexArray.h"
#include "Render/Shader.h"
#include "tools/config.h"

class Shape: public Drawable {
protected:
	VertexBuffer* vb;					// holds the point information
	IndexBuffer* ib;					// tells OpenGL how to draw the tri's
	VertexArray* va;					// tells OpenGL meta information on our shape
	Shader* shader;						// the program that tells OpenGL how to draw our shape

	std::vector<float> positions;		// how many total points we have
	std::vector<unsigned int> indices;	// how OpenGL should read our points
	//
	void bind();
	void calculateBounds() override;
	void scaleBounds(float scaleX, float scaleY) override;
	void resize(float width, float height);
public:
	Shape (float width, float height);
	~Shape ();
	void draw () override;
	//
	void setScale (float scaleX, float scaleY);
	void setScaleX(float scale);
	void setScaleY(float scale);
	void setTranslation (float dx, float dy);
};