#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D spriteTexture;

void main(){
  fragColor = texture(spriteTexture, ex_Tex);
}
