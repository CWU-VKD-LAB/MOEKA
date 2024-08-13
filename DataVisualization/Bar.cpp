#include "Bar.h"

Bar::Bar(int classValue): Shape{config::barWidth, config::barHeight} {
	/*if (classValue > config::maxClassValue) {
		classValue = config::maxClassValue;
	}
	float val = 1.0f - .8f * ((float)classValue/config::maxClassValue);
	color.x = val;
	color.y = val;
	color.z = val;
	*/
	color = &config::classColors[classValue];
	//if (classValue == 1)
	//{
	//	// GRB
	//	color.x = 0;
	//	color.y = 1; // red
	//	color.z = 0;
	//}
	//else
	//{
	//	color.x = 1; // green
	//	color.y = 0;
	//	color.z = 0;
	//}
}
