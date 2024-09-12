#include "Bar.h"

Bar::Bar(int classValue): Shape{config::barWidth, config::barHeight} {
	color = &config::classColors[classValue];
}
