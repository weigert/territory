#version 330 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec3 in_Color;

//Uniforms
uniform mat4 model;
uniform mat4 vp;
uniform mat4 dbmvp;
uniform mat4 back;

uniform float lightstrength;
uniform vec3 lightcol;
uniform vec3 lightpos;
uniform vec3 lookdir;

out vec3 ex_Normal;
out vec4 ex_Color;

out vec4 ex_OrgColor;

out vec3 ex_FragPos;
out vec4 ex_ScreenPos;
out vec2 ex_Position;
out vec4 ex_Shadow;

vec4 gouraud(){

	float diffuse = clamp(dot(in_Normal, normalize(lightpos)), 0.1,  0.7);
	float ambient = 0.1;
	float specular = 0.2*pow(max(dot(normalize(lookdir), normalize(reflect(lightpos, in_Normal))), 0.0), 16);

	return vec4(lightcol*lightstrength*(diffuse + ambient + specular), 1.0f);

}

void main(void) {

	ex_FragPos = (model * vec4(in_Position, 1.0f)).xyz;
	gl_Position = vp * vec4(ex_FragPos, 1.0f);
	ex_ScreenPos = gl_Position;

	ex_Normal = in_Normal;

	ex_Shadow = dbmvp * back * vec4(ex_FragPos, 1.0f);
	ex_Position = ((gl_Position.xyz / gl_Position.w).xy * 0.5 + 0.5 );


	ex_OrgColor = vec4(in_Color, 1.0);
	ex_Color = vec4(in_Color, 1.0)*gouraud();

}
