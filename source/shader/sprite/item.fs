#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D spriteTexture;
uniform float index;

void main(){
  //Scale the Actual Guy
  float height = 1.0/3.0; //Height of a single guy
  vec2 test = ex_Tex;
  test.y = test.y / 3 + height * index;

  vec4 color = texture(spriteTexture, test);
  if(color.a == 0.0){
    discard;
  }
  else{
    fragColor = color;
  }
}
