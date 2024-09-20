#include "Shape.h"


Shape::Shape (float width, float height) {
    resize(width, height);
}

void Shape::resize (float width, float height) {
    float x = width / 2.0f;
    float y = height / 2.0f;

    positions = {
         x,  y,
        -x,  y,
        -x, -y,
         x, -y
    };
    indices = {
        0, 1, 2,
        2, 3, 0
    };

    // if any of these exist, delete them first before reallocating.
    if (va != nullptr) {
        delete(va);
    }
    if (vb != nullptr) {
        delete(vb);
    }
    if (ib != nullptr) {
        delete(ib);
    }
    if (shader != nullptr) {
        delete(shader);
    }

    va = new VertexArray{};                     // create our vertex array object, that holds the metadata of our points
    vb = new VertexBuffer{ 1, &positions };     // holds our points
    ib = new IndexBuffer{ 1, &indices };        // allow for reuse of points

    // set the layout
    VertexLayout vl{};
    vl.setNumOfFloats(2);
    va->addBuffer(*vb, vl);

    // set up our shader
    shader = new Shader{ "resources/shaders/basic.glsl" };
    shader->remove();

    // clear our bindings
    clearBinds();

    // define our bounds
    float tempx, tempy;
    for (int a = 0; a < positions.size(); a += 2) {
        tempx = positions.at(a);
        tempy = positions.at((long long)a + 1);
        if (tempx > originalMaxBoundsX) originalMaxBoundsX = tempx;
        if (tempy > originalMaxBoundsY) originalMaxBoundsY = tempy;
        if (tempx < originalMinBoundsX) originalMinBoundsX = tempx;
        if (tempy < originalMinBoundsY) originalMinBoundsY = tempy;
    }
    calculateBounds();
    setTranslation(config::windowX / 2, config::windowY / 2);
    setScale(1, 1);
    shader->setUniformMat4f("projMatrix", config::proj);
}

// dispose of our heap allocated variables.
Shape::~Shape () {
    delete(vb);
    delete(ib);
    delete(va);
    delete(shader);
}

void Shape::draw () {
    bind();
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}
void Shape::calculateBounds() {
    float tempx, tempy;
    for (int a = 0; a < positions.size(); a += 2) {
        tempx = positions.at(a);
        tempy = positions.at((long long)a + 1);
        if (tempx > maxBoundsX) maxBoundsX = tempx;
        if (tempy > maxBoundsY) maxBoundsY = tempy;
        if (tempx < minBoundsX) minBoundsX = tempx;
        if (tempy < minBoundsY) minBoundsY = tempy;
    }
}
void Shape::bind() {
    va->bind();
    vb->bind();
    ib->bind();
    shader->use();
    shader->setUniform4f("uColor", color->x, color->y, color->z, color->w);
}

// transformations
void Shape::setTranslation(float dx, float dy) {
    x = dx;
    y = dy;
    dx = 2 * (dx / config::windowX) - 1;
    dy = 2 * (dy / config::windowY) - 1;
    translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(dx, -dy, 0));
    shader->use();
    shader->setUniformMat4f("posMatrix", translateMatrix);
};

void Shape::setScale(float scaleX, float scaleY) {
    totalScaleX = scaleX;
    totalScaleY = scaleY;
    scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, 1));
    shader->use();
    shader->setUniformMat4f("scaleMatrix", scaleMatrix);
    scaleBounds(totalScaleX, totalScaleY);
}

void Shape::setScaleX(float scale) {
    totalScaleX = scale;
    scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(totalScaleX, totalScaleY, 1));
    shader->use();
    shader->setUniformMat4f("scaleMatrix", scaleMatrix);
    scaleBounds(totalScaleX, totalScaleY);
}

void Shape::setScaleY(float scale) {
    totalScaleY = scale;
    scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(totalScaleX, totalScaleY, 1));
    shader->use();
    shader->setUniformMat4f("scaleMatrix", scaleMatrix);
    scaleBounds(totalScaleX, totalScaleY);
}



void Shape::scaleBounds(float scaleX, float scaleY) {
    calculateBounds();
    minBoundsX = originalMinBoundsX * (scaleX);
    maxBoundsX = originalMaxBoundsX * (scaleX);
    minBoundsY = originalMinBoundsY * (scaleY);
    maxBoundsY = originalMaxBoundsY * (scaleY);
}
