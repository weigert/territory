#version 330 core

layout (location = 0) in vec3 in_Quad;
layout (location = 1) in vec2 in_Tex;

out vec2 ex_Tex;

//uniform mat4 mvp;

uniform float width;      //Height of Spritesheet
uniform float height;     //Width of Spritesheet

uniform float nframe;     //Current Frame (X-Position)
uniform float animation;  //Current Animation (Y-Position)

uniform mat4 vp;
uniform mat4 model;

void main(){

  ex_Tex = (in_Tex+vec2(nframe, animation))/vec2(width, height);
  gl_Position = vp * model * vec4(in_Quad, 1.0);

}
