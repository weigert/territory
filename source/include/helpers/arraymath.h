//Functions to do Arraymath with
namespace helper{

  int getIndex(glm::vec2 p, glm::vec2 s){
    //Return the Correct Index
    if(glm::all(glm::lessThan(p, s)) && glm::all(glm::greaterThanEqual(p, glm::vec2(0)))){
      return p.x*s.y+p.y;
    }
    return -1;
  }

  void set(float a[], float b[], int size){
    for(int i = 0; i < size; i++){
      a[i] = b[i];
    }
  }

  void abs(float a[], int size){
    for(int i = 0; i < size; i++){
      if(a[i] < 0){
        a[i] = -a[i];
      }
    }
  }

  void add(float a[], float b[], int size){
    for(int i = 0; i < size; i++){
      a[i] += b[i];
    }
  }

  void add(float a[], float x, int size){
    for(int i = 0; i < size; i++){
      a[i] += x;
    }
  }

  void multiply(float a[], float b[], int size){
    for(int i = 0; i < size; i++){
      a[i] *= b[i];
    }
  }

  void multiply(float a[], float x, int size){
    for(int i = 0; i < size; i++){
      a[i] *= x;
    }
  }

  void divide(float a[], float b[], int size){
    for(int i = 0; i < size; i++){
      a[i] /= b[i];
    }
  }

  //Compute a gradient at a point
  glm::vec2 gradient(float field[], glm::vec2 _pos, glm::vec2 s){
    //Differential Values
    float dx = 0;
    float dy = 0;

    //Make sure neighborhood exists
    int ax = helper::getIndex(glm::mod(_pos+glm::vec2(1,0)+s, s), s);
    int bx = helper::getIndex(glm::mod(_pos+glm::vec2(-1,0)+s, s), s);
    int ay = helper::getIndex(glm::mod(_pos+glm::vec2(0,1)+s, s), s);
    int by = helper::getIndex(glm::mod(_pos+glm::vec2(0,-1)+s, s), s);

    dx = (field[ax] - field[bx])/2;
    dy = (field[ay] - field[by])/2;

    return glm::vec2(dx, dy);
  }

  glm::vec2 curvature(float field[], glm::vec2 _pos, glm::vec2 s){
    //Differential Values
    float dx = 0;
    float dy = 0;

    //Make sure neighborhood exists
    int ax = helper::getIndex(glm::mod(_pos+glm::vec2(1,0)+s, s), s);
    int bx = helper::getIndex(glm::mod(_pos+glm::vec2(-1,0)+s, s), s);
    int ay = helper::getIndex(glm::mod(_pos+glm::vec2(0,1)+s, s), s);
    int by = helper::getIndex(glm::mod(_pos+glm::vec2(0,-1)+s, s), s);

    dx = (field[ax] + field[bx] - 2*field[helper::getIndex(_pos, s)])/4;
    dy = (field[ay] + field[by] - 2*field[helper::getIndex(_pos, s)])/4;

    return glm::vec2(dx, dy);
  }


  void diffuse(float field[], float mu, glm::vec2 s){
    //Perform Diffusion Pass
    for(int i = 0; i < s.x; i++){
      for(int j = 0; j < s.y; j++){
        //Get Curvature
        field[helper::getIndex(glm::vec2(i,j), s)] += mu*curvature(field, glm::vec2(i, j), s).x;
        field[helper::getIndex(glm::vec2(i,j), s)] += mu*curvature(field, glm::vec2(i, j), s).y;
      }
    }
  }

  void shape(float g[], float o[], glm::vec2 s){
    //Copmute a Kernel??
    int n[5][2] = {{0,0}, {0,1}, {0,-1}, {1,0}, {-1,0}};

    //Given o, we need to produce a g
    for(int i = 0; i < s.x; i++){
      for(int j = 0; j < s.y; j++){
        glm::vec2 _pos = glm::vec2(i, j);
        int x = helper::getIndex(_pos, s);

        //Set it to 0 by default.
        g[x] = 0;

        if(o[x] != 0){
          //Get all his neighbors
          for(int k = 0; k < 4; k++){
            int ax = helper::getIndex(glm::mod(_pos+glm::vec2(n[k][0],n[k][1])+s, s), s);
            //Add to that guys value too
            g[ax] += o[x]/5;
          }
        }
      }
    }
  }

  void normalize(float a[], int size){
    float max = 0;
    float min = 1;
    for(int i = 0; i < size; i++){
      max = (a[i] > max)?a[i]:max;
      min = (a[i] < min)?a[i]:min;
    }
    for(int i = 0; i < size; i++){
      a[i] -= min;
      a[i] /= (max-min);
    }
  }

  bool inRange(glm::vec3 a, glm::vec3 b, glm::vec3 range){
    return glm::all(glm::lessThanEqual(glm::abs(a-b), range));
  }

  bool inModRange(glm::vec3 a, glm::vec3 b, glm::vec3 range, int mod){
    glm::vec3 _a = glm::floor(a/glm::vec3(mod));
    glm::vec3 _b = glm::floor(b/glm::vec3(mod));
    return glm::all(glm::lessThanEqual(glm::abs(_a - _b), range));
  }

  //End of Namespace
}

/*
================================================================================
                      VOLUME FUNCTIONS (FOR BUILDINGS)
================================================================================
*/

struct Volume{
  //Bounding Box Corners
  glm::vec3 a;
  glm::vec3 b;

  void translate(glm::vec3 shift);
  int getVol();
};

//Get the Total Volume
int Volume::getVol(){
  return abs((b.x-a.x)*(b.y-a.y)*(b.z-a.z));
}

//Move the Corners of the Bounding Box
void Volume::translate(glm::vec3 shift){
  a += shift;
  b += shift;
}

Volume boundingBox(Volume v1, Volume v2){
  //New Bounding Volume
  Volume bb;

  //Determine the Lower Corner
  bb.a.x = (v1.a.x < v2.a.x)?v1.a.x:v2.a.x;
  bb.a.y = (v1.a.y < v2.a.y)?v1.a.y:v2.a.y;
  bb.a.z = (v1.a.z < v2.a.z)?v1.a.z:v2.a.z;

  //Determine the Upper Corner
  bb.b.x = (v1.b.x > v2.b.x)?v1.b.x:v2.b.x;
  bb.b.y = (v1.b.y > v2.b.y)?v1.b.y:v2.b.y;
  bb.b.z = (v1.b.z > v2.b.z)?v1.b.z:v2.b.z;

  return bb;
}

//Compute the Volume Overlap in 3 Directions
glm::vec3 overlapVolumes(Volume v1, Volume v2){
  //Compute Bounding Box for the two volumes
  Volume bb = boundingBox(v1, v2);

  //Compute the Overlap
  glm::vec3 ext1 = glm::abs(v1.b - v1.a);  //Extent of v1 in 3 directions
  glm::vec3 ext2 = glm::abs(v2.b - v2.a);  //Extent of v2 in 3 directions
  glm::vec3 extbb = glm::abs(bb.b - bb.a); //Extent of the bounding box

  //Overlap is given
  return ext1 + ext2 - extbb;
}

//Some cost function to evaluate
int volumeCostFunction(std::vector<Volume> rooms){
  //Metric
  int metric[6] = {
    0, //Positive Intersection Volume
    0, //Negative Intersection Volume
    0, //Surface Area Touching Vertically
    0, //Surface Area Touching Horizontally
    0, //Amount of Area Touching Floor
    0};//Amount of Volume Below the Floor

  int weight[6] = {2, 4, -5, -5, -5, 5};

  //Compute the Energy of a configuration of rooms
  for(unsigned int i = 0; i < rooms.size(); i++){

    //Comparison Metrics to Other Rooms
    for(unsigned int j = 0; j < rooms.size(); j++){
      //If it's the same room, ignore.
      if(i == j) continue;

      //Compute the Overlap between two volumes.
      glm::vec3 overlap = overlapVolumes(rooms[i], rooms[j]);

      //Positive Intersection Volume (if any of them are 0, it's 0)
      glm::vec3 posOverlap = glm::clamp(overlap, glm::vec3(0), overlap);
      metric[0] += glm::abs(posOverlap.x*posOverlap.y*posOverlap.z); //Ignore Negative Values

      //Negative Intersection Volume
      glm::vec3 negOverlap = glm::clamp(overlap, overlap, glm::vec3(0));
      metric[1] += glm::abs(negOverlap.x*negOverlap.y*negOverlap.z); //Ignore Negative Values

      //Top Surface Contact
      if(overlap.y == 0){
        metric[2] += overlap.x*overlap.z;
      }
      //Side Surface Contact (X)
      if(overlap.x == 0){
        //This can still be 0, if it is corners touching, i.e. overlap.z = 0
        metric[3] += overlap.z*overlap.y;
      }
      //Side Surface Contact (Z)
      if(overlap.z == 0){
        //This can still be 0, if it is corners touching, i.e. overlap.x = 0
        metric[4] += overlap.x*overlap.y;
      }
    }

    //We need to know if any rooms are touching the floor.
    if(rooms[i].a.y == 0){
      //If the rooms is larger, than it is more important that it touches the floor.
      metric[4] += rooms[i].a.x*rooms[i].a.z;
    }

    //Check if we have volume below the floor!
    if(rooms[i].a.y < 0){
      //Check if the whole volume is below the surface
      if(rooms[i].b.y < 0){
        metric[5] += rooms[i].getVol();
      }
      else{
        metric[5] += abs(rooms[i].a.y)*(rooms[i].b.z-rooms[i].a.z)*(rooms[i].b.x-rooms[i].a.x);
      }
    }
  }

  //Return Metric * Weights
  return metric[0]*weight[0] + metric[1]*weight[1] + metric[2]*weight[2] + metric[3]*weight[3] + metric[4]*weight[4] + metric[5]*weight[5];
}

/*
================================================================================
                      IOSTREAM OPERATORS FOR GLM
================================================================================
*/

std::ostream & operator << (std::ostream &out, const glm::vec2 &vec)
{
    out << "X: "<<vec.x<<", Y: "<<vec.y;
    return out;
}

std::ostream & operator << (std::ostream &out, const glm::vec3 &vec)
{
    out << "X: "<<vec.x<<", Y: "<<vec.y<<", Z: "<<vec.z;
    return out;
}
