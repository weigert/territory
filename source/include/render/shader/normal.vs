#version 130
// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
in vec3 in_Position;
in vec4 in_Color;
in vec3 in_Normal;

//Uniforms
uniform mat4 mvp;
uniform mat4 model;

out vec3 ex_Normal;
out vec4 ex_Color;

void main(void) {
	// Set the position to the one defined in our vertex array
	gl_Position = mvp * model * vec4(in_Position, 1.0f);
	ex_Normal = in_Normal;
	ex_Color = in_Color;
}
