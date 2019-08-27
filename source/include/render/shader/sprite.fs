#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D spriteTexture;

void main()
{
    //Read the Texture and Output to Color
    fragColor = texture(spriteTexture, ex_Tex);
    //fragColor = vec4(depthValue*vec3(1.0f), 1.0f);
    //fragColor = vec4(1.0f);
}
