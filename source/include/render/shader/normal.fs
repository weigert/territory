#version 130
in vec3 ex_Normal;
in vec4 ex_Color;
out vec4 fragColor;

void main(void) {
  fragColor = vec4((ex_Normal.r+2*ex_Normal.g+3*ex_Normal.b)/3, 0.0, 0.0, 1.0);
}
