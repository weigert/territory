#version 460 core

in vec2 ex_Tex;
out vec4 fragColor;

//uniform float lightStrength;
uniform sampler2D spriteTexture;

void main(){

  fragColor = texture(spriteTexture, ex_Tex);

  if(fragColor.a == 0.0)
    discard;

  //else{
  //  fragColor = min(vec4(vec3(lightStrength), 1.0)*color, color);
  //}
}
