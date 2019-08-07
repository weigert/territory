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
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, shadowCoord.xy).r;
    float currentDepth = shadowCoord.z;

    float shadow = (currentDepth-0.001 > closestDepth) ? .5 : 0.0;

    return vec4(vec3(1-shadow), 1.0f);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(void) {
  //Lighting
  float lightFactor = clamp( dot( ex_Normal , normalize(lightPos)*1.6 ), 0.3,  1.0);

  //*vec4(lightColor, ex_Position - vec2(600.0f, 400.0f));
  vec4 objColor = ex_Color*vec4(lightCol*lightFactor, 1.0f);
  //vec4 grain = vec4(rand(ex_WorldPos.xy)*vec3(1.0f), 1.0f);
  //vec4 grain = 0.1*vec4(rand(ex_WorldPos.xy)*vec3(1.0f), 1.0f);

  //vec4 objColor = mix(ex_Color, lightCol, lightFactor);
  fragColor = shade()*objColor;//mix(fogColor, objColor, fogFactor);

  //Fog
  //float z = length(abs(ex_Position - vec2(0.5f, 0.5f)))*3;
  //float fogFactor = 1-clamp(1-exp(-(z-0.5)*10), 0, 1);
  //vec4 fogColor = vec4(0.7f, 0.86f, 0.89f, 1.0f);
}
