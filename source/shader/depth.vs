#version 330 core

layout (location = 0) in vec3 in_Position;

uniform mat4 model;
uniform mat4 dvp;

void main(void) {
	vec3 ex_FragPos = (model * vec4(in_Position, 1.0f)).xyz;
	gl_Position = dvp * vec4(ex_FragPos, 1.0f);
}
