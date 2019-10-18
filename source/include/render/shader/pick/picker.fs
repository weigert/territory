#version 130
out vec4 fragColor;
in vec3 ex_Position;
uniform mat4 model;
uniform mat4 camera;
uniform mat4 projection;
uniform vec3 worldsize;
uniform vec3 viewpos;

void main(){
  vec3 pos = ceil((model*vec4(ex_Position, 1.0)).xyz-0.5);

  vec3 offset = mod(viewpos, vec3(16));
  fragColor = vec4(1.0);
}
