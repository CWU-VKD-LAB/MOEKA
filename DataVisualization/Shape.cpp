#include "Shape.h"


void Shape::setup () {
    // create our vertex array object, that holds the metadata of our points
    va = new VertexArray{};

    // holds our points
    vb = new VertexBuffer{1, &positions};
    
    // allow for reuse of points
    ib = new IndexBuffer{1, &indices};

    // set the layout
    VertexLayout vl{};
    vl.setNumOfFloats(2);
    va->addBuffer(*vb, vl);

    // set up our shader
    shader = new Shader{"resources/shaders/basic.glsl"};
    shader->remove();

    // clear our bindings
    clearBinds();

    // define our bounds
    float tempx , tempy;
    for (int a = 0; a < positions.size(); a+=2) {
        tempx = positions.at(a);
        tempy = positions.at(a+1);
        if (tempx > maxBoundsX) maxBoundsX = tempx;
        if (tempy > maxBoundsY) maxBoundsY = tempy;
        if (tempx < minBoundsX) minBoundsX = tempx;
        if (tempy < minBoundsY) minBoundsY = tempy;
    }
}

void Shape::bind () {
    va->bind();
    vb->bind();
    ib->bind();
    shader->use();
    shader->setUniform4f("uColor", color.x, color.y, color.z, color.w);
}
void Shape::draw() {
    // bindings
    bind();

    // draw
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}
void Shape::setPosition (float m_x, float m_y) {
    x = m_x;
    y = m_y;
}
float Shape::getPositionX () {
    return x;
}
float Shape::getPositionY () {
    return y;
}

void Shape::setKValue(int val) {
    kValue = val;
}

Shape::~Shape() {
    delete(vb);
    delete(ib);
    delete(va);
    delete(shader);
}

int Shape::getKValue() {
    return kValue;
}

