#version 130
in vec3 in_Position;

uniform mat4 dvp;
uniform mat4 model;

void main(void) {
	// Set the position to the one defined in our vertex array
	gl_Position = dvp * model * vec4(in_Position, 1.0f);
}
