#include "IndexBuffer.h"

IndexBuffer::IndexBuffer (int m_count, std::vector<unsigned int>* indices): count(m_count){
    // generate our index buffers and give it data
    GLCall(glGenBuffers(count, &id));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(unsigned int), indices->data(), GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer () {
    GLCall(glDeleteBuffers(count, &id));
}

void IndexBuffer::bind () const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, id));
}

void IndexBuffer::unbind () const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}