#include "Bar.h"

Bar::Bar(int classValue): Shape{config::barWidth, config::barHeight} {
	classVal = classValue;
	color = &config::classColors[classValue];
}
