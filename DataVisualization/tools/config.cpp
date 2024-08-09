#include "config.h"

namespace config {
	float color[3]{1, 1, 1};
	std::vector<std::string> options {
		"Align chains vertically", 
		"Show layer borders", 
		"Show element borders",
		"Show element borders in one color", 
		"Show bush up", 
		"Highlight a chain", 
		"Sort elements using numerical values", 
		"Align chains", 
		"Move an element by dragging", 
		"Move a chain by dragging", 
		"Automatically center Hansel Chains", 
		"Expand elements", 
		"Show monotonicity violations", 
		"Change expanded to real elements", 
		"Expand monotonicity", 
		"Show 3D view", 
		"Show 3D plot view", 
		"Show 3D compressed view", 
		"Show initial position of disk"
	};
	float windowX = 800;
	float windowY = 800;
	glm::mat4 proj = glm::ortho(-windowX / 2, windowX / 2, windowY / 2, -windowY / 2, 1.0f, -1.0f);

}
