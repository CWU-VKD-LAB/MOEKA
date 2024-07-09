#pragma once
#include <vector>
#include <GL/glew.h>

struct VertexBufferElement {
	// how many of the type 
	unsigned int count;
	// GLtype
	unsigned int type;
	bool normalize;

	// how many bytes a type has
	static unsigned int sizeOfType (unsigned int type) {
		switch (type) {
			case GL_FLOAT: return 4;
			case GL_UNSIGNED_INT: return 4;
			case GL_UNSIGNED_BYTE: return 1;
		}
		return 0;
	}
};

class VertexLayout {
private:
	unsigned int stride = 0;
	std::vector<VertexBufferElement> list{};
public:
	// how many floats constitute a point in our render.
	void setNumOfFloats(unsigned int count) {
		list.push_back({ count, GL_FLOAT, false });
		stride = count * sizeof(GLfloat);
	}

	// return the list of elements
	inline const std::vector<VertexBufferElement> getElements () const& {
		return list;
	}

	// how much we should move per point
	inline unsigned int getStride () const {
		return stride;
	}
};
