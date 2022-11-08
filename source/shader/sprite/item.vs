#version 130
in vec3 in_Quad;
in vec2 in_Tex;
out vec2 ex_Tex;

uniform mat4 mvp;

void main(){
  //Set Ex-Tex:
  ex_Tex = in_Tex;
  gl_Position = mvp*vec4(in_Quad, 1.0);
}
