#version 330

uniform sampler2D imageTexture;

in vec2 ex_Tex;
out vec4 fragColor;

void main(){
    fragColor = texture(imageTexture, ex_Tex);
}
