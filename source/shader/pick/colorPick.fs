#version 130
uniform vec3 un_Color;
out vec4 fragColor;

void main(){
  fragColor = vec4(un_Color, 1.0);
}
