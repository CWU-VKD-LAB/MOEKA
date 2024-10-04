#include "Bar.h"

// creates a bar object that stores its class value and color.
Bar::Bar(int classValue): Shape{config::barWidth, config::barHeight} {
	classVal = classValue;
	color = &config::classColors[classValue];
	// shape.h field that tells us if the shape is a bar. yes, it looks dumb, but it was an easier than
	// dealing with casting pointers and such.
	isBar = true;
}

// draw the bar using individual color or class color.
void Bar::draw() {
	if (useIndividualColor) {
		color = &individualColor;
	}
	else {
		color = &config::classColors[classVal];
	}
	bind();
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}
