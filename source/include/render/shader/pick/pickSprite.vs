#version 130
in vec3 in_Position;
out vec3 ex_Position;

uniform mat4 mvp;

void main(){
  //Set Ex-Tex:
  ex_Position = in_Position;
  gl_Position = mvp*vec4(in_Position, 1.0);
}
