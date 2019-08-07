#version 130
// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
in vec3 in_Position;
in vec4 in_Color;
in vec3 in_Normal;

//Uniforms
uniform mat4 model;
uniform mat4 camera;
uniform mat4 projection;
uniform mat4 dmvp;
uniform mat4 dbmvp;

// We output the ex_Color variable to the next shader in the chain
out vec4 ex_Color;
out vec3 ex_Normal;
out vec2 ex_Position;
out vec3 ex_WorldPos;
out vec4 shadowCoord;
out vec3 ex_FragPos;

//Extra Stuff


void main(void) {
	// Set the position to the one defined in our vertex array
	gl_Position = projection * camera * model * vec4(in_Position, 1.0f);

	//Projected Into Light ViewSpace
	shadowCoord = dbmvp * model * vec4(in_Position, 1.0f);

  // Pass the color on to the fragment shader
	ex_Color = in_Color;
	ex_Normal = in_Normal;

	//In ModelSpace
	ex_FragPos = vec3(model*vec4(in_Position, 1.0f));

	//In WorldSpace
	ex_WorldPos = vec3(camera*model*vec4(in_Position, 1.0f));

	//Distance to Center
	ex_Position = ((gl_Position.xyz / gl_Position.w).xy * 0.5 + 0.5 );
}
