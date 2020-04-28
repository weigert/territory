#include "../forward/world.fwd.h"

struct Plant{
  Plant(int i, glm::ivec2 d){
    index = i;
    pos = glm::vec2(i/d.y, i%d.y);
  };

  Plant(glm::vec2 p, glm::ivec2 d){
    pos = p;
    index = (int)p.x*d.y+(int)p.y;
  };

  glm::vec2 pos;
  int index;
  float size = 0.5;
  const float maxsize = 1.0;
  const float rate = 0.05;

  void grow();
  void root(double* density, glm::ivec2 dim, double factor);

  Plant& operator=(const Plant& o){
    if(this != &o){  //Self Check
      pos = o.pos;
      index = o.index;
      size = o.size;
    }
    return *this;
  };
};


struct Drop{
  //Construct Particle at Position
  Drop(glm::vec2 _pos){ pos = _pos; }
  Drop(glm::vec2 _p, glm::ivec2 dim, double v){
    pos = _p;
    int index = _p.x*dim.y+_p.y;
    volume = v;
  }

  //Properties
  int index;
  glm::vec2 pos;
  glm::vec2 speed = glm::vec2(0.0);
  double volume = 1.0;   //This will vary in time
  double sediment = 0.0; //Sediment concentration

  //Parameters
  const float dt = 1.2;
  const double density = 2.0;  //This gives varying amounts of inertia and stuff...
  const double evapRate = 0.01;
  const double depositionRate = 0.1;
  const double minVol = 0.01;
  const double friction = 0.1;
  const double volumeFactor = 100.0; //"Water Deposition Rate"

  //Sedimenation Process
  void descend(double* h, double* path, double* pool, bool* track, double* pd, glm::ivec2 dim, double scale);
  void flood(double* h, double* pool, glm::ivec2 dim);
};


class Terrain{
public:
  Terrain(World* _w){
    w = _w;
  };

  World* w;                             //World Pointer

  glm::ivec2 dim = glm::vec2(256, 256);    //Size of the heightmap array
  glm::ivec3 rescale = glm::ivec3(4, 3, 4);  //Scale to World-Size

  double scale = 80.0;                  //"Physical" Height scaling of the map
  double heightmap[256*256] = {0.0};    //Flat Array
  double waterpath[256*256] = {0.0};    //Water Path Storage (Rivers)
  double waterpool[256*256] = {0.0};    //Water Pool Storage (Lakes / Ponds)
  float steepness = 0.6;

  std::vector<Plant> trees;
  double plantdensity[256*256] = {0.0}; //Density for Plants

  void generate();

  //
  void perlin();
  void flat();

  //
  void erode(int cycles);               //Erode with N Particles
  void grow();
};

void Plant::grow(){
  size += rate*(maxsize-size);
};

void Plant::root(double* density, glm::ivec2 dim, double f){

  //Can always do this one
  density[index]       += f*1.0;

  if(pos.x > 0){
    //
    density[index - 256] += f*0.6;      //(-1, 0)

    if(pos.y > 0)
      density[index - 257] += f*0.4;    //(-1, -1)

    if(pos.y < 256-1)
      density[index - 255] += f*0.4;    //(-1, 1)
  }

  if(pos.x < 256-1){
    //
    density[index + 256] += f*0.6;    //(1, 0)

    if(pos.y > 0)
      density[index + 255] += f*0.4;    //(1, -1)

    if(pos.y < 256-1)
      density[index + 257] += f*0.4;    //(1, 1)
  }

  if(pos.y > 0)
    density[index - 1]   += f*0.6;    //(0, -1)

  if(pos.y < 256-1)
    density[index + 1]   += f*0.6;    //(0, 1)
}


glm::vec3 surfaceNormal(int index, double* h, glm::ivec2 dim, double scale){

  glm::vec3 n = glm::vec3(0.0);

  //Two large triangles adjacent to the plane (+Y -> +X) (-Y -> -X)
  for(int i = 1; i <= 1; i++){
    n += (1.0f/(float)i*i)*glm::cross(glm::vec3(0.0, scale*(h[index+i]-h[index]), i), glm::vec3( i, scale*(h[index+i*dim.y]-h[index]), 0.0));
    n += (1.0f/(float)i*i)*glm::cross(glm::vec3(0.0, scale*(h[index-i]-h[index]),-i), glm::vec3(-i, scale*(h[index-i*dim.y]-h[index]), 0.0));
    n += (1.0f/(float)i*i)*glm::cross(glm::vec3( i, scale*(h[index+i*dim.y]-h[index]), 0.0), glm::vec3(0.0, scale*(h[index-i]-h[index]),-i));
    n += (1.0f/(float)i*i)*glm::cross(glm::vec3(-i, scale*(h[index-i*dim.y]-h[index]), 0.0), glm::vec3(0.0, scale*(h[index+i]-h[index]), i));
  }

  return glm::normalize(n);
}

void Terrain::erode(int cycles){

  //Track the Movement of all Particles
  bool track[dim.x*dim.y] = {false};

  //Do a series of iterations!
  for(int i = 0; i < cycles; i++){

    //Spawn New Particle
    glm::vec2 newpos = glm::vec2(rand()%(int)dim.x, rand()%(int)dim.y);
    Drop drop(newpos);


    int spill = 5;
    while(drop.volume > drop.minVol && spill != 0){

      drop.descend(heightmap, waterpath, waterpool, track, plantdensity, dim, scale);

      if(drop.volume > drop.minVol)
        drop.flood(heightmap, waterpool, dim);

      spill--;
    }
  }

  //Update Path
  double lrate = 0.01;
  for(int i = 0; i < dim.x*dim.y; i++)
    waterpath[i] = (1.0-lrate)*waterpath[i] + lrate*((track[i])?1.0:0.0);

}

void Terrain::grow(){

  //Random Position
  {
    int i = rand()%(dim.x*dim.y);
    glm::vec3 n = surfaceNormal(i, heightmap, dim, scale);

    if( waterpool[i] == 0.0 &&
        waterpath[i] < 0.2 &&
        n.y > 0.8 ){

        Plant ntree(i, dim);
        ntree.root(plantdensity, dim, 1.0);
        trees.push_back(ntree);
    }
  }

  //Loop over all Trees
  for(int i = 0; i < trees.size(); i++){

    //Grow the Tree
    trees[i].grow();

    //Spawn a new Tree!
    if(rand()%50 == 0){
      //Find New Position
      glm::vec2 npos = trees[i].pos + glm::vec2(rand()%9-4, rand()%9-4);

      //Check for Out-Of-Bounds
      if( npos.x >= 0 && npos.x < dim.x &&
          npos.y >= 0 && npos.y < dim.y ){

        Plant ntree(npos, dim);
        glm::vec3 n = surfaceNormal(ntree.index, heightmap, dim, scale);

        if( waterpool[ntree.index] == 0.0 &&
            waterpath[ntree.index] < 0.2 &&
            n.y > 0.8 &&
            (double)(rand()%1000)/1000.0 > plantdensity[ntree.index]){
              ntree.root(plantdensity, dim, 1.0);
              trees.push_back(ntree);
            }
      }
    }

    //If the tree is in a pool or in a stream, kill it
    if(waterpool[trees[i].index] > 0.0 ||
       waterpath[trees[i].index] > 0.2 ||
       rand()%1000 == 0 ){ //Random Death Chance
         trees[i].root(plantdensity, dim, -1.0);
         trees.erase(trees.begin()+i);
         i--;
       }
  }

};

void Drop::descend(double* h, double* p, double* b, bool* track, double* pd, glm::ivec2 dim, double scale){

  glm::ivec2 ipos;

  while(volume > minVol){

    //Initial Position
    ipos = pos;
    int ind = ipos.x*dim.y+ipos.y;

    //Add to Path
    track[ind] = true;

    glm::vec3 n = surfaceNormal(ind, h, dim, scale);

    //Effective Parameter Set
    /* Higher plant density means less erosion */
    double effD = depositionRate*max(0.0, 1.0-pd[ind]);

    /* Lower Friction, Lower Evaporation in Streams
    makes particles prefer established streams -> "curvy" */
    double effF = friction*(1.0-0.5*p[ind]);
    double effR = evapRate*(1.0-0.2*p[ind]);

    //Newtonian Mechanics
    glm::vec2 acc = glm::vec2(n.x, n.z)/(float)(volume*density);
    speed += dt*acc;
    pos   += dt*speed;
    speed *= (1.0-dt*effF);

    //New Position
    int nind = (int)pos.x*dim.y+(int)pos.y;

    //Out-Of-Bounds
    if(!glm::all(glm::greaterThanEqual(pos, glm::vec2(0))) ||
       !glm::all(glm::lessThan((glm::ivec2)pos, dim))){
         volume = 0.0;
         break;
       }

    //Particle is not accelerated
    if(p[nind] > 0.3 && length(acc) < 0.01)
      break;

    //Particle enters Pool
    if(b[nind] > 0.0)
      break;

    //Mass-Transfer (in MASS)
    double c_eq = max(0.0, glm::length(speed)*(h[ind]-h[nind]));
    double cdiff = c_eq - sediment;
    sediment += dt*effD*cdiff;
    h[ind] -= volume*dt*effD*cdiff;

    //Evaporate (Mass Conservative)
    sediment /= (1.0-dt*effR);
    volume *= (1.0-dt*effR);
  }
};

void Drop::flood(double* h, double* p, glm::ivec2 dim){

  //Current Height
  index = (int)pos.x*dim.y + (int)pos.y;
  double plane = h[index] + p[index];
  double initialplane = plane;

  //Floodset
  std::vector<int> set;
  int fail = 10;

  //Iterate
  while(volume > minVol && fail){

    set.clear();
    int size = dim.x*dim.y;
    bool tried[size] = {false};

    int drain;
    bool drainfound = false;

    std::function<void(int)> floodfill = [&](int i){
      //Out of Bounds
      if(i < 0 || i >= size) return;

      //Position has been tried
      if(tried[i]) return;
      tried[i] = true;

      //Wall / Boundary
      if(plane < h[i] + p[i]) return;

      //Drainage Point
      if(initialplane > h[i] + p[i]){

        //No Drain yet
        if(!drainfound)
          drain = i;

        //Lower Drain
        else if( p[drain] + h[drain] < p[i] + h[i] )
          drain = i;

        drainfound = true;
        return;
      }

      set.push_back(i);
      floodfill(i+1);
      floodfill(i+dim.y+1);  //Fill Diagonals
      floodfill(i+dim.y);    //Fill Neighbors
      floodfill(i+dim.y-1);
      floodfill(i-1);
      floodfill(i-dim.y-1);
      floodfill(i-dim.y);
      floodfill(i-dim.y+1);
    };

    //Perform Flood
    floodfill(index);

    //Drainage Point
    if(drainfound){

      //Set the Drop Position and Evaporate
      pos = glm::vec2(drain/dim.y, drain%dim.y);

      //Set the New Waterlevel (Slowly)
      double drainage = 0.1;
      plane = (1.0-drainage)*initialplane + drainage*(h[drain] + p[drain]);

      //Compute the New Height
      for(auto& s: set)
        p[s] = (plane > h[s])?(plane-h[s]):0.0;

      //Remove Sediment
      sediment *= 0.1;
      break;
    }

    //Get Volume under Plane
    double tVol = 0.0;
    for(auto& s: set)
      tVol += volumeFactor*(plane - (h[s]+p[s]));

    //We can partially fill this volume
    if(tVol <= volume && initialplane < plane){

      //Raise water level to plane height
      for(auto& s: set)
        p[s] = plane - h[s];

      //Adjust Drop Volume
      volume -= tVol;
      tVol = 0.0;
    }

    //Plane was too high.
    else fail--;

    //Adjust Planes
    initialplane = (plane > initialplane)?plane:initialplane;
    plane += 0.8*(volume-tVol)/(double)set.size()/volumeFactor;
  }

  //Couldn't place the volume (for some reason)- so ignore this drop.
  if(fail == 0)
    volume = 0.0;
}
