#version 130
// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
in vec3 in_Position;
in vec4 in_Color;
in vec3 in_Normal;

//Uniforms
uniform mat4 model;
uniform mat4 projectionCamera;

// We output the ex_Color variable to the next shader in the chain
out vec4 ex_Color;
out vec3 ex_Normal;
out vec3 ex_FragPos;

void main(void) {
	// Set the position to the one defined in our vertex array
	gl_Position = projectionCamera * model * vec4(in_Position, 1.0f);

  // Pass the color on to the fragment shader
	ex_Color = in_Color;
	ex_Normal = in_Normal;
	ex_FragPos = vec3(model*vec4(in_Position, 1.0f));	//Model Space
}
