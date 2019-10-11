#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;
uniform float width;
uniform float height;
uniform bool vert;

//Compute the Gauss-Blur coefficients
float gauss(int offset, float stddev){
  //Return the Gauss Coefficient
  return 1.0/sqrt(2.0*3.14159265*pow(stddev,2))*exp(-pow(offset/stddev,2)/2.0);
}

vec4 blur(int samples, float stddev){
  //Empty Blurred Color
  vec4 result = vec4(vec3(0.0), 1.0);

  //Vertical or Horizontal Bluer
  if(!vert){
    for(int i = -samples; i <= samples; i++){
      result += gauss(i, stddev)*texture(imageTexture, vec2(ex_Tex.s + i/width, ex_Tex.t));
    }
  }
  else{
    for(int i = -samples; i <= samples; i++){
      result += gauss(i, stddev)*texture(imageTexture, vec2(ex_Tex.s, ex_Tex.t + i/height));
    }
  }

  //Return the Blurred Version
  return result;
}

void main()
{
    fragColor = blur(10, 5.0);
}
