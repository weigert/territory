#version 130
//Lighting Settings
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec3 lookDir;
uniform float lightStrength;

//Sampler for the ShadowMap
uniform sampler2D shadowMap;

//Extra Stuff
uniform bool transparent;
uniform vec3 volPosA;
uniform vec3 volPosB;
uniform bool _grain;
uniform sampler2D reflection;
uniform int clip;

//IO
in vec4 ex_Color;
in vec3 ex_Normal;
in vec2 ex_Position;
in vec4 ex_Shadow;
in vec3 ex_FragPos;
in vec4 ex_OriginalColor;
out vec4 fragColor;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float gridSample(int size){
  //Stuff
  float shadow = 0.0;
  float currentDepth = ex_Shadow.z;

  //Compute Bias
  float m = 1-dot(ex_Normal, normalize(lightPos));
  float bias = mix(0.002, 0.2*m, pow(m, 5));

  for(int x = -size; x <= size; ++x){
      for(int y = -size; y <= size; ++y){
          float pcfDepth = texture(shadowMap, ex_Shadow.xy + vec2(x, y) / textureSize(shadowMap, 0)).r;
          shadow += currentDepth-bias > pcfDepth ? 1.0 : 0.0;
      }
  }
  //Normalize
  shadow/=((2*size+1)*(2*size+1)*2.0);
  return shadow;
}

vec4 shade(){
    //Shadow Value
    float shadow = 0.0;
    if(greaterThanEqual(ex_Shadow.xy, vec2(0.0f)) == bvec2(true) && lessThanEqual(ex_Shadow.xy, vec2(1.0f)) == bvec2(true))
      shadow = gridSample(1);

    //Sample the Shadow Value from Texture
    return vec4(vec3(1-shadow), 1.0f);
}

void main(void) {
  //Check for Transparency Window
  if(transparent){
    if(all(greaterThanEqual(ex_FragPos+0.5, min(volPosA, volPosB))) && all(lessThanEqual(ex_FragPos-0.5, max(volPosA, volPosB)))){
      discard; return;
    }
  }

  //Color!
  fragColor = shade()*ex_Color;

  //Reflection
  if(ex_FragPos.y == clip-0.5 && ex_OriginalColor == vec4(0.3f, 0.57f, 0.67f, 1.0f))
    fragColor = mix(fragColor, texture(reflection, vec2(1-ex_Position.x, ex_Position.y)), 1-dot(vec3(0,-1,0), normalize(lookDir)));

  //Texture Grain
  if(_grain) fragColor += 0.1*vec4(rand(ex_Position.xy)*vec3(1.0f), 1.0f);
}
