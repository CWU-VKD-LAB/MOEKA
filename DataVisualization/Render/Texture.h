#pragma once
#include "../tools/Util.h"

class Texture {
public:
	unsigned int id;
	std::string filePath;
	unsigned char* buffer;
	int width, height, bpp;

	Texture (const std::string& path);
	~Texture ();
	
	void bind (unsigned int slot = 0);
	void unbind ();

	inline int getWidth() {return width;};
	inline int getHeight() {return height;};
	
};
