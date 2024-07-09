#include "Util.h"


void GLClearError () {
	// loop until we clear all errors.
	while (glGetError() != GL_NO_ERROR);
}

bool GLCheckError (const char* function, const char* file, int line) {
	// check if we have an error.
	GLenum error;
	while (error = glGetError()) {
		std::cout << "OpenGL Error: " << error << "\n\t" 
			<< function << "\n\t"
			<< file << ":" << line << std::endl;
		return false;
	}
	return true;
}

void clearBinds () {
	glBindVertexArray(0);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
