//Functions to do Arraymath with
namespace helper{

  int getIndex(glm::vec2 p, glm::vec2 s){
    //Return the Correct Index
    if(glm::all(glm::lessThan(p, s)) && glm::all(glm::greaterThanEqual(p, glm::vec2(0)))){
      return p.x*s.y+p.y;
    }
    return -1;
  }

  //3D Index
  int getIndex(glm::vec3 p, glm::vec3 s){
    if(glm::all(glm::lessThan(p, s)) && glm::all(glm::greaterThanEqual(p, glm::vec3(0)))){
      return p.x*s.y*s.z+p.y*s.z+p.z;
    }
    return -1;
  }

  glm::vec3 getPos(int index, glm::vec3 s){
    int z = index % (int)s.x;
    int y = (int)(index / s.x) % (int)s.y;
    int x = index / ( s.x * s.y );
    return glm::vec3(x, y, z);
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

double max(double a, double b){
  return (a > b)?a:b;
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

std::ostream & operator << (std::ostream &out, const glm::vec4 &vec)
{
    out << "X: "<<vec.x<<", Y: "<<vec.y<<", Z: "<<vec.z<<", W: "<<vec.a;
    return out;
}

std::ostream & operator << (std::ostream &out, const glm::ivec2 &vec)
{
    out << "X: "<<vec.x<<", Y: "<<vec.y;
    return out;
}

std::ostream & operator << (std::ostream &out, const glm::ivec3 &vec)
{
    out << "X: "<<vec.x<<", Y: "<<vec.y<<", Z: "<<vec.z;
    return out;
}

std::ostream & operator << (std::ostream &out, const glm::ivec4 &vec)
{
    out << "X: "<<vec.x<<", Y: "<<vec.y<<", Z: "<<vec.z<<", W: "<<vec.a;
    return out;
}
