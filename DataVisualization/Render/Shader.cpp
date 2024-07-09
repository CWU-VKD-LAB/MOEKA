#include "Shader.h"

Shader::Shader(const std::string& file): filePath(file) {
	FullShader s = parseShader(file);
	id = createShader(s.vertex, s.frag);
}

Shader::~Shader() {
	GLCall(glDeleteProgram(id));
}


// tells OpenGL to use this shader.
void Shader::use() {
	GLCall(glUseProgram(id));
}


// tells OpenGL to stop using this shader.
void Shader::remove() {
	GLCall(glUseProgram(0));
}


// resets a shader on an object
void Shader::recompileShader (const std::string& file) {
	FullShader s = parseShader(file);
	id = createShader(s.vertex, s.frag);
}


// set the values of a uniform.
void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
	use();
	GLCall(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
}

void Shader::setUniformMat4f(const std::string& name, glm::mat4& matrix) {
	use();
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_TRUE, &matrix[0][0]);
}


// searches if we have already found this uniform before, if so, we return it instead of searching again for performance reasons.
unsigned int Shader::getUniformLocation(const std::string& name) {
	if (cache.find(name) != cache.end()) {
		return cache[name];
	}
	GLCall(int location = glGetUniformLocation(id, name.c_str()));
	if (location == -1) {
		std::cout << "Warning: uniform failed to load." << std::endl;
	}
	cache[name] = location;
	return location;
}


// given a source file, we create a new shader program
unsigned int Shader::compileShader(const std::string& source, unsigned int type) {
	unsigned int id = glCreateShader(type);
	const char* src = &source[0];
	// what shader program, how many, address to the first character, how much of the string to use. if null, then null terminated.
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	// iv = integer - vector/array/pointer
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	// if we failed to create a Shader, print the reason why into the console and return 0.
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*) alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile shader " << (type == GL_VERTEX_SHADER ? "vertex " : "fragment ") << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}


// creates the shader, then attaches it to the program and cleans up aftewards.
unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader(vertexShader, GL_VERTEX_SHADER);
	unsigned int fs = compileShader(fragShader, GL_FRAGMENT_SHADER);

	//
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	// cleanup
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


// splits up a file that contains multiple shaders into multiple strings
FullShader Shader::parseShader(const std::string& path) {
	std::ifstream stream(path);

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		// if we find the #shader, set the type of shader it is as the ShaderType.
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::VERTEX;
			}
			else if (line.find("frag") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else {
			ss[(int)type] << line << "\n";
		}
	}

	// return a struct with our source codes split up.
	return { ss[0].str(), ss[1].str() };
}


