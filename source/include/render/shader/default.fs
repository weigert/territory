#version 130
uniform vec3 lightCol;
uniform vec3 lightPos;

in vec4 ex_Color;
in vec3 ex_Normal;
in vec2 ex_Position;
in vec4 shadowCoord;
in vec3 ex_FragPos;
in vec3 ex_WorldPos;

//Sampler for the ShadowMap
uniform sampler2D shadowMap;

out vec4 fragColor;

vec4 shade()
{
    float shadow = 0.0;
    shadow /= 9.0;
    //Make sure shadow is inside the shadowCoord Box, otherwise don't draw shadows!
    if(greaterThanEqual(shadowCoord.xy, vec2(0.0f)) == bvec2(true) && lessThanEqual(shadowCoord.xy, vec2(1.0f)) == bvec2(true)){
      // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
      float currentDepth = shadowCoord.z;
      float bias = max(0.05 * (1.0 - dot(ex_Normal, lightPos)), 0.005);
      vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
      int a = 2;
      for(int x = -a; x <= a; ++x)
      {
          for(int y = -a; y <= a; ++y)
          {
              float pcfDepth = texture(shadowMap, shadowCoord.xy + vec2(x, y) * texelSize).r;
              shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
          }
      }
      shadow/=((2*a+1)*(2*a+1)*2.0);
    }

    return vec4(vec3(1-shadow), 1.0f);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(void) {
  //Lighting
  float lightFactor = clamp( dot(ex_Normal, normalize(lightPos)*2.0), 0.3,  1.0);
  vec4 objColor = ex_Color*vec4(lightCol*lightFactor, 1.0f);
  vec4 grain = 0.1*vec4(rand(ex_WorldPos.xy)*vec3(1.0f), 1.0f);
  fragColor = shade()*objColor+grain;
}
