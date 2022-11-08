#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;
uniform sampler2D depthTexture;

uniform bool grain;
uniform bool fog;
uniform vec3 fogcolor;

float ease(float x){
  float z = (0.9)/(0.5)-0.1/0.5;
  return z*pow(x,2)-(0.25*z-0.1)/0.5*x;
}

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(){

  fragColor = texture(imageTexture, ex_Tex);

  //Distance Fog
  if(fog){
    float depthVal = clamp(texture(depthTexture, ex_Tex).r, 0.0, 1.0);
    if(depthVal < 1.0){
      float d = (depthVal-0.4)/0.2;
      fragColor = mix(fragColor, vec4(fogcolor, 1.0), d*d);
    }
  }

  /*
  if(true){

    float d0 = clamp(texture(depthTexture, ex_Tex).r, 0.0, 1.0);
    float d1 = clamp(texture(depthTexture, ex_Tex+vec2(0.001,0)).r, 0.0, 1.0);
    float d2 = clamp(texture(depthTexture, ex_Tex+vec2(-0.001,0)).r, 0.0, 1.0);
  //  float d2 = clamp(texture(depthTexture, ex_Tex).r, 0.0, 1.0);
  //  float d3 = clamp(texture(depthTexture, ex_Tex).r, 0.0, 1.0);

    if(abs(d1 - d0) > 0.0001) fragColor = vec4(1,1,1,1);
    if(abs(d2 - d0) > 0.0001) fragColor = vec4(1,1,1,1);
  }
  */

//
//  float depthVal


  //Texture Grain
  //if(grain)
  //  fragColor += 0.1*vec4(rand(ex_Tex)*vec3(1.0f), 1.0f);

}
