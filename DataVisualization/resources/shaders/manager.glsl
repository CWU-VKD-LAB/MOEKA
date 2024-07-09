#shader vertex
#version 410 core

layout(location = 0) in vec4 position;
out vec4 test;

void main() {
	gl_Position = position;
};

#shader frag
#version 410 core

layout(location = 0) out vec4 color;

uniform vec4 uColor;

void main() {
	color = uColor;
};