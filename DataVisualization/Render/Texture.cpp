#include "Texture.h"
#include "../stb/stb_image.h"

Texture::Texture (const std::string& path): id(0), buffer(nullptr), filePath(path), width(0), height(0), bpp(0) {
	// load image to the cpu.
	stbi_set_flip_vertically_on_load(0);
	buffer = stbi_load(path.c_str(), &width, &height, &bpp, 4);
	if (buffer == NULL) {
		std::cout << "Failed to load texture" << std::endl;
	}
	
	// create and bind the texture on the gpu.
	GLCall(glGenTextures(1, &id));
	GLCall(glBindTexture(GL_TEXTURE_2D, id));

	// tell openGL some parameters.
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));

	// tell openGL how to read it, then unbind.
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	// remove from cpu.
	if (buffer) {
		stbi_image_free(buffer);
	}
}

Texture::~Texture () {
	GLCall(glDeleteTextures(1, &id));
}

void Texture::bind (unsigned int slot) {
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, id));
}

void Texture::unbind () {
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
