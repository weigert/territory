#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform float lightStrength;
uniform sampler2D spriteTexture;

void main(){
  vec4 color = texture(spriteTexture, ex_Tex);
  if(color.a == 0.0){
    discard;
  }
  else{
    fragColor = min(vec4(vec3(lightStrength), 1.0)*color, color);
  }
}
