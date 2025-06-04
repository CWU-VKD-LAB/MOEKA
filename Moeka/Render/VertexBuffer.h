#pragma once
#include <GL/glew.h>
#include "../tools/Util.h"
#include <vector>

class VertexBuffer {
	unsigned int id;
	int count;
public:
	VertexBuffer (int m_count, std::vector<float>* positions);
	~VertexBuffer ();
	void bind () const;
	void unbind () const;
};
