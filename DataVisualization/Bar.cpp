#include "Bar.h"

Bar::Bar(int classValue): Shape{config::barWidth, config::barHeight} {
	if (classValue > config::maxClassValue) {
		classValue = config::maxClassValue;
	}
	float val = 1.0f - .8f * ((float)classValue/config::maxClassValue);
	color.x = val;
	color.y = val;
	color.z = val;
}
