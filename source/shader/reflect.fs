#version 130
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform float lightStrength;

in vec4 ex_Color;
in vec3 ex_Normal;
in vec3 ex_FragPos;

uniform int clip;

//Sampler for the ShadowMap
out vec4 fragColor;

void main(void) {
   if (ex_FragPos.y < clip-0.5) discard;

   //Lighting
   float diffuse = clamp(dot(ex_Normal, normalize(lightPos)), 0.1,  0.7);
   float ambient = 0.1;

   fragColor = ex_Color*vec4(lightCol*lightStrength*(diffuse + ambient), 1.0f);
}
