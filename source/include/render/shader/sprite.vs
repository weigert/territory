#version 130
in vec3 in_Quad;
in vec2 in_Tex;
out vec2 ex_Tex;

uniform mat4 mvp;
uniform float nframe;
uniform float width;
uniform float height;
uniform float animation;

void main(){
  //Set Ex-Tex:
  ex_Tex = in_Tex;

  //Alter the X
  if(in_Tex.x == 1.0){
    ex_Tex = ex_Tex / vec2(width, 1.0f);
  }

  //Alter the Y
  if(in_Tex.y == 1.0f){
    ex_Tex = ex_Tex / vec2(1.0f, height);
  }

  //Translate everything
  ex_Tex = ex_Tex + vec2(nframe / width, animation / height);

  gl_Position = mvp*vec4(in_Quad, 1.0);
}
