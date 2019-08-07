#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D shadowMap;

void main()
{
    //Read the Texture and Output to Color
    float depthValue = texture(shadowMap, ex_Tex).r;
    fragColor = vec4(depthValue*vec3(1.0f), 1.0f);
}
