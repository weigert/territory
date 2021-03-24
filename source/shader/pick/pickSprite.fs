#version 130
in vec3 ex_Position;
out vec4 fragColor;

void main(){
  fragColor = vec4(ex_Position, 1.0);
}
