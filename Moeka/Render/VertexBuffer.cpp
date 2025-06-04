#include "VertexBuffer.h"


VertexBuffer::VertexBuffer (int m_count, std::vector<float>* positions): count(m_count) {
    // create a vertex buffer
    GLCall(glGenBuffers(count, &id));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, id));
    GLCall(glBufferData(GL_ARRAY_BUFFER, positions->size() * sizeof(float), positions->data(), GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer () {
    GLCall(glDeleteBuffers(count, &id));
}

void VertexBuffer::bind () const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, id));
}

void VertexBuffer::unbind () const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
