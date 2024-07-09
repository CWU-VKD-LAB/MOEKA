#pragma once
#include <string>
#include <GL/glew.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include "../tools/Util.h"
#include <glm/glm.hpp>

struct FullShader {
	std::string vertex;
	std::string frag;
};

class Shader {
private:
	unsigned int id;
	std::unordered_map<std::string, int> cache{};
	unsigned int getUniformLocation (const std::string& name);
	unsigned int compileShader(const std::string& source, unsigned int type);
	unsigned int createShader(const std::string& vertexShader, const std::string& fragShader);
	FullShader parseShader(const std::string& path);
	//
public:
	std::string filePath;

	Shader (const std::string& file);
	~Shader ();
	
	//
	void use ();
	void remove ();

	// uniforms
	void setUniform4f (const std::string& name, float v0, float v1, float v2, float v3);
	void setUniformMat4f (const std::string& name, glm::mat4& matrix);
	void recompileShader (const std::string& name);
};
