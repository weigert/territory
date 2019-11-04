#version 130
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec3 lookDir;
uniform bool _grain;
uniform float lightStrength;

in vec4 ex_Color;
in vec3 ex_Normal;
in vec2 ex_Position;
in vec4 shadowCoord;
in vec3 ex_FragPos;
in vec3 ex_WorldPos;
in vec4 ex_TexPos;

//Extra Stuff
uniform bool transparent;
uniform vec3 volPosA;
uniform vec3 volPosB;

//Sampler for the ShadowMap
uniform sampler2D shadowMap;

//Reflection
uniform sampler2D reflection;
uniform int clip;

out vec4 fragColor;

vec4 shade()
{
    float shadow = 0.0;
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
  //Check for Transparency Window
  if(transparent){
    if( all(greaterThanEqual(ex_FragPos+0.5, min(volPosA, volPosB))) && all(lessThanEqual(ex_FragPos-0.5, max(volPosA, volPosB)))){
      discard; return;
    }
  }

  vec3 reflectDir = reflect(lightPos, ex_Normal);

  //Lighting
  float diffuse = clamp(dot(ex_Normal, normalize(lightPos)), 0.1,  0.7);
  float ambient = 0.1;
  float specular = pow(max(dot(normalize(lookDir), normalize(reflectDir)), 0.0), 32);

  //Color!
  fragColor = ex_Color*vec4(lightCol*lightStrength*(diffuse + ambient + specular), 1.0f);

  //Shadow Mapping
  fragColor = shade()*fragColor;

  //Reflection
  if(ex_FragPos.y == clip-0.5 && ex_Color == vec4(0.3f, 0.57f, 0.67f, 1.0f)){
    fragColor = mix(fragColor, texture(reflection, vec2(1-ex_Position.x, ex_Position.y)), 1-dot(vec3(0,-1,0), normalize(lookDir)));
  }

  //Texture Grain
  if(_grain) fragColor += 0.1*vec4(rand(ex_WorldPos.xy)*vec3(1.0f), 1.0f);
}
