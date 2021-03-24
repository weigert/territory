/*

Properties of the Scene

*/

namespace scene{
using namespace glm;

float daytime = 0;

int LOD = 1;

vec3 skycol = vec3(0.6, 0.9f, 0.8f);

vec3 lightcol = vec3(1.0f, 1.0f, 0.9f);
float lightstrength = 1.5;

vec3 clickColorBlock = vec3(0.0f, 1.0f, 0.0f);
vec3 hoverColorBlock = vec3(1.0f, 0.0, 0.0f);

vec3 renderdist = vec3(1);

//Render Parameters
int blur = 0; //Degree of Blur
bool fog = true;
bool grain = false;
vec3 fogcolor = vec3(0.6, 0.9f, 0.8f);


//Rotation and View
int lookstate = 3;


vec3 lightpos = vec3(1.0f, 3.0f, 2.0f);

mat4 depthModelMatrix = mat4(1.0);
mat4 dproj = ortho<float>(-300,300,-300,300,-200,200);
mat4 dview = lookAt(lightpos, vec3(0,0,0), vec3(0,1,0));

mat4 dvp = dproj*dview;

mat4 bias = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
);

}
