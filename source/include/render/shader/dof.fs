#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;
uniform sampler2D depthTexture;
uniform float width;
uniform float height;
uniform bool vert;
uniform bool _fog;
uniform int _blur;
uniform float mousex;
uniform float mousey;

uniform vec3 fogColor;

//Compute the Gauss-Blur coefficients
float gauss(int offset, float stddev){
  //Return the Gauss Coefficient
  return 1.0/sqrt(2.0*3.14159265*pow(stddev,2))*exp(-pow(offset/stddev,2)/2.0);
}

float ease(float x){
  float z = (0.9)/(0.5)-0.1/0.5;
  return z*pow(x,2)-(0.25*z-0.1)/0.5*x;
}

vec4 blur(int samples, float stddev){
  //Empty Blurred Color
  vec4 result = vec4(vec3(0.0), 1.0);
  float norm = 0.0;

  //Vertical or Horizontal Bluer
  if(!vert){
    for(int i = -samples; i <= samples; i++){
      float w = gauss(i, stddev);
      norm += w;
      result += w*texture(imageTexture, vec2(ex_Tex.s + i/width, ex_Tex.t));
    }
  }
  else{
    for(int i = -samples; i <= samples; i++){
      float w = gauss(i, stddev);
      norm += w;
      result += w*texture(imageTexture, vec2(ex_Tex.s, ex_Tex.t + i/height));
    }
  }

  //Return the Blurred Version
  return result/norm;
}

vec4 depthblur(int samples, float stddev){
  float depth = abs(texture(depthTexture, vec2(mousex, mousey)).r - texture(depthTexture, ex_Tex).r);
  if(depth > 0.01){
    return blur(samples, depth*stddev);
  }
  else return texture(imageTexture, ex_Tex);
}

void main(){
    //Set fragment color, either by blur or depthblur
    float depthVal = clamp(texture(depthTexture, ex_Tex).r, 0.0, 1.0);
    fragColor = texture(imageTexture, ex_Tex);
    //if(_blur!=0) fragColor = depthblur(_blur, float(_blur)/2.0);
    //if(depthVal < 1.0 && _fog) fragColor = mix(fragColor, vec4(fogColor, 1.0), ease(depthVal));
}
