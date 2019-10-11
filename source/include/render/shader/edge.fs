#version 130
in vec2 ex_Tex;
out vec4 fragColor;
uniform float width;
uniform float height;
uniform sampler2D imageTexture;
uniform sampler2D depthTexture;

float sobel(sampler2D _texture){
  //We need to compute an edge detect...
  float result = 0.0;
  //Sobel Edge Detect
  result += (-1)*texture(_texture, vec2(ex_Tex.s-1/width, ex_Tex.t+1/height)).r;
  result += (-2)*texture(_texture, vec2(ex_Tex.s-1/width, ex_Tex.t)).r;
  result += (-1)*texture(_texture, vec2(ex_Tex.s-1/width, ex_Tex.t-1/height)).r;
  result += (+1)*texture(_texture, vec2(ex_Tex.s+1/width, ex_Tex.t+1/height)).r;
  result += (+2)*texture(_texture, vec2(ex_Tex.s+1/width, ex_Tex.t)).r;
  result += (+1)*texture(_texture, vec2(ex_Tex.s+1/width, ex_Tex.t-1/height)).r;

  result += (-1)*texture(_texture, vec2(ex_Tex.s+1/width, ex_Tex.t+1/height)).r;
  result += (-2)*texture(_texture, vec2(ex_Tex.s, ex_Tex.t+1/height)).r;
  result += (-1)*texture(_texture, vec2(ex_Tex.s-1/width, ex_Tex.t+1/height)).r;
  result += (+1)*texture(_texture, vec2(ex_Tex.s+1/width, ex_Tex.t-1/height)).r;
  result += (+2)*texture(_texture, vec2(ex_Tex.s, ex_Tex.t-1/height)).r;
  result += (+1)*texture(_texture, vec2(ex_Tex.s-1/width, ex_Tex.t-1/height)).r;

  return result;
}

void main(){
    //Result from Edge-Detect
    float result = sobel(depthTexture);

    //If we are at an edge, we become black
    if(abs(result) > 0.007) fragColor = vec4(vec3(0.0),1.0);
    else fragColor = texture(imageTexture, ex_Tex);
}
