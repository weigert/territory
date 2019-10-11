#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D spriteTexture;

void main(){
    vec4 test = texture(spriteTexture, ex_Tex);
    if(test.a == 0){
      fragColor = vec4(0.0);
    }
    else{
      fragColor = texture(spriteTexture, ex_Tex);    
    }
}
