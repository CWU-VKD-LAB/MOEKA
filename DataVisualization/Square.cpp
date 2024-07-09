#include "Square.h"

int Square::count = 0;

Square::Square (float x1, float y1, float x2, float y2, std::string name) {
    positions = {
         x1,  y1,
         x2,  y1,
         x2,  y2,
         x1,  y2
    };

    indices = {
        0, 1, 2,
        2, 3, 0
    };
    count++;
    id = count;
    setup();
}

Square::Square (int width, int height, std::string name) {
    float x = width/2.0;
    float y = height/2.0;

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
    count++;
    id = count;
    setup();
}


Square::~Square () {

}