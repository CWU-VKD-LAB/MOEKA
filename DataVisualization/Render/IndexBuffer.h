#pragma once
#include <GL/glew.h>
#include "../tools/Util.h"
#include <vector>

class IndexBuffer {
	unsigned int id;
	int count;
public:
	IndexBuffer (int m_count, std::vector<unsigned int>* indices);
	~IndexBuffer ();
	void bind () const;
	void unbind () const;
};
