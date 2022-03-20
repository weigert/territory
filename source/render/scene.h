/*
================================================================================
                    Properties of the Scene for Rendering
================================================================================
*/

namespace scene{
using namespace glm;

//LIGHTING PROPERTIES

float daytime = 0;

vec3 skycol = vec3(0.6, 0.9f, 0.8f);
vec3 lightcol = vec3(1.0f, 1.0f, 0.9f);
float lightstrength = 1.5;
vec3 lightpos = vec3(1.0f, 2.0f, 2.0f);
vec3 fogcolor = vec3(1.0f);

mat4 depthModelMatrix = mat4(1.0);
mat4 dproj = ortho<float>(-300,300,-300,300,-200,200);
mat4 dview = lookAt(lightpos, vec3(0,0,0), vec3(0,1,0));

mat4 dvp = dproj*dview;

const mat4 bias = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
);

void computelighting(float time){

  //Compute Lighting Conditions
  //scene::skycol = color::bezier(ease::cubic(scene::daytime), color::skycolors);
  //scene::lightstrength = 1.4*ease::quartic(scene::daytime)+0.1;
  //scene::fogcolor = glm::vec4(ease::quartic(scene::daytime), ease::quartic(scene::daytime), ease::quartic(scene::daytime), 1.0);

  //Compute Depth Projection Matrices
  dview = lookAt(lightpos, vec3(0), glm::vec3(0,1,0));
  dvp = dproj*dview;

}



//MESHING / CHUNK RENDERING / ZOOMING PROPERTIES

int LOD = 1;
int SCALE = 1;

vec3 renderdist = vec3(1);

int QUAD = 2400;
int MAXBUCKET = 20*20*10*8;

int oldzoomstate = 0;
int zoomstate = 0;

bool changezoomstate(float camrad){

  //Get Current Zoomstate!
  if(cam::type == cam::ORTHO){
    if(camrad >= 5.0f)    zoomstate = 0;
    else if(camrad >= 2.5f)  zoomstate = 1;
    else                  zoomstate = 2;
  }
  else{
    if(camrad <= 600.0f)        zoomstate = 0;
    else if(camrad <= 1200.0f)  zoomstate = 1;
    else                        zoomstate = 2;
  }

  if(zoomstate == oldzoomstate) return false;

  //Update LOD
  LOD = pow(2, zoomstate);
  oldzoomstate = zoomstate;

  //Change Render Parameters
  if(zoomstate == 2){

    renderdist = vec3(32, 12, 32);
    dproj = ortho<float>(-800,800,-800,800,-600,600);
    dvp = dproj*dview;

  }
  if(scene::zoomstate == 1){

    renderdist = vec3(10, 8, 10);
    dproj = ortho<float>(-300,300,-300,300,-300,300);
    dvp = dproj*dview;

  }
  if(scene::zoomstate == 0){

    renderdist = vec3(7, 4, 7);
    dproj = ortho<float>(-200,200,-200,200,-300,300);
    dvp = dproj*dview;

  }

  return true;

}


//Rotation and View
int lookstate = 3;

vec3 clickColorBlock = vec3(0.0f, 1.0f, 0.0f);
vec3 hoverColorBlock = vec3(1.0f, 0.0, 0.0f);

int blur = 0; //Degree of Blur
bool fog = false;
bool grain = true;

}
