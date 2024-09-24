#include "Bar.h"

Bar::Bar(int classValue): Shape{config::barWidth, config::barHeight} {
	classVal = classValue;
	color = &config::classColors[classValue];
	isBar = true;
}

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
