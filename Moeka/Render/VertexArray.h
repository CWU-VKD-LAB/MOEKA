#pragma once
#include <../tools/Util.h>
#include "VertexBuffer.h"
#include "VertexLayout.h"


class VertexArray {

private:
	std::vector<VertexBuffer*> buffers{};

public:
	unsigned int id;
	VertexArray ();
	~VertexArray ();

	void bind () const;
	void unbind () const;
	void addBuffer (VertexBuffer& vb, VertexLayout& layout);
};
