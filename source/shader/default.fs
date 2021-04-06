#version 330 core

in vec4 ex_Color;
in vec3 ex_Normal;

in vec4 ex_OrgColor;
in vec3 ex_FragPos;
in vec2 ex_Position;
in vec4 ex_Shadow;

out vec4 fragColor;

uniform vec3 lightcol;
uniform vec3 lightpos;
uniform vec3 lookdir;
uniform float lightstrength;

in vec4 ex_ScreenPos;

uniform bool shading;

/*

uniform bool transparent;
uniform vec3 volPosA;
uniform vec3 volPosB;
uniform sampler2D reflection;
uniform int clip;

*/

uniform sampler2D shadowMap;
uniform mat4 dbmvp;

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float gridSample(int size){

  float shadow = 0.0;
  float currentDepth = ex_Shadow.z;

  //Compute Bias
  float m = 1-dot(ex_Normal, normalize(lightpos));
  float bias = mix(0.002, 0.2*m, pow(m, 5));

  for(int x = -size; x <= size; ++x){
      for(int y = -size; y <= size; ++y){
          float pcfDepth = texture(shadowMap, ex_Shadow.xy + vec2(x, y) / textureSize(shadowMap, 0)).r;
          shadow += currentDepth-bias > pcfDepth ? 1.0 : 0.0;
      }
  }

  shadow/=((2*size+1)*(2*size+1)*2.0);
  return shadow;

}

vec4 shade(){

  float shadow = 0.0f;

  if(greaterThanEqual(ex_Shadow.xy, vec2(0.0f)) == bvec2(true) && lessThanEqual(ex_Shadow.xy, vec2(1.0f)) == bvec2(true))
    shadow = gridSample(1);

  return vec4(vec3(1.0f-shadow), 1.0f);
}

void main(void) {

  fragColor = ex_Color;

  //Shading
  if(shading)
    fragColor = shade()*ex_Color;

}
