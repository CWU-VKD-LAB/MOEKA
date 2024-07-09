#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// MSVC function to create a break if the assertion fails. this is specific to the visual studio compiler.
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLCheckError(#x, __FILE__, __LINE__));

void GLClearError ();
bool GLCheckError (const char* function, const char* file, int line);
void clearBinds ();
