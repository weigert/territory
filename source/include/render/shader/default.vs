#version 130
// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
in vec3 in_Position;
in vec4 in_Color;
in vec3 in_Normal;

//Lighting
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec3 lookDir;
uniform float lightStrength;

//Uniforms
uniform mat4 model;
uniform mat4 projectionCamera;
uniform mat4 dbmvp;

// We output the ex_Color variable to the next shader in the chain
out vec4 ex_Color;
out vec4 ex_OriginalColor;
out vec3 ex_Normal;
out vec2 ex_Position;
out vec4 ex_Shadow;
out vec3 ex_FragPos;

void main(void) {
	//Position Calculations
	ex_FragPos = (model * vec4(in_Position, 1.0f)).xyz;
	ex_Shadow = dbmvp * vec4(ex_FragPos, 1.0f);
	gl_Position = projectionCamera * vec4(ex_FragPos, 1.0f);
	ex_Position = ((gl_Position.xyz / gl_Position.w).xy * 0.5 + 0.5 );
	ex_Normal = in_Normal;

	//Color Calculations - Per Vertex! Not Fragment.
	float diffuse = clamp(dot(in_Normal, normalize(lightPos)), 0.1,  0.7);
	float ambient = 0.1;
	float specular = pow(max(dot(normalize(lookDir), normalize(reflect(lightPos, in_Normal))), 0.0), 64);
	ex_Color = in_Color*vec4(lightCol*lightStrength*(diffuse + ambient + specular), 1.0f);
	ex_OriginalColor = in_Color;
}
