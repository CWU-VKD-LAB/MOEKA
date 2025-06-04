#include "VertexArray.h"


VertexArray::VertexArray () {
	// generate our vertex array and set our ID
	GLCall(glGenVertexArrays(1, &id));
	GLCall(glBindVertexArray(id));
}

VertexArray::~VertexArray () {
	GLCall(glDeleteVertexArrays(1, &id));
}

// adds a vertex buffer to this vertex array and defines its layout.
void VertexArray::addBuffer (VertexBuffer& vb, VertexLayout& layout) {
	// bind the Vertex Array and Vertex Buffer before we do work on them.
	bind();
	vb.bind();
	
	// our layout should have the information for each buffer.
	const auto& elements = layout.getElements();
	unsigned int offset = 0;

	// set each layout defined
	for (int a = 0; a < elements.size(); a++) {
		const auto& element = elements[a];
		// tell OpenGL how our memory is stored via glVertexAttribPointer, then enable it.
		GLCall(glVertexAttribPointer(a, element.count, element.type, element.normalize, layout.getStride(), (const void*)offset));
		GLCall(glEnableVertexAttribArray(a));

		// if we have multiple buffers, we need to have an offset.
		offset += element.count * element.sizeOfType(element.type);
	}
}

// call when we want to draw our shape
void VertexArray::bind () const {
	GLCall(glBindVertexArray(id));
}

// call when we want to unbind our shape
void VertexArray::unbind() const {
	GLCall(glBindVertexArray(0));
}
