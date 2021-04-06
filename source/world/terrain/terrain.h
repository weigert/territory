#ifndef TERRITORY_TERRAIN
#define TERRITORY_TERRAIN

#include "../../helper/ease.h"
#include <noise/noise.h>

#include "vegetation.h"
#include "water.h"

class Terrain{
public:

  Terrain(int _SEED, ivec3 _D){
    SEED = _SEED;
    D = _D;
    heightmap = new double[D.x*D.z];
    waterpath = new double[D.x*D.z];
    waterpool = new double[D.x*D.z];
    plantdensity = new double[D.x*D.z];
    dim = glm::ivec2(D.x, D.z);
    scale = D.y;
  };

  ~Terrain(){
    delete[] heightmap;
  };

  Blueprint blueprint;

  int SEED = 0;
  ivec3 D;

  glm::ivec2 dim;                       //Size of the heightmap array
  double scale = 80.0;                  //"Physical" Height scaling of the map

  glm::ivec3 rescale = glm::ivec3(1);   //Scale to World-Size

  std::vector<Plant> trees;

  double* heightmap;
  double* waterpath;    //Water Path Storage (Rivers)
  double* waterpool;    //Water Pool Storage (Lakes / Ponds)
  double* plantdensity; //Density for Plants


  float steepness = 0.6;

  void generate(int ncycles);
  //
  void perlin(int SEED, vec3 dim, bool(World::*f)(Blueprint& print), World* w);

  void perlin(int SEED, vec3 dim);
//  void perlin(int SEED, vec3 dim);
  void flat(vec2 s);
  //
  void erode(int cycles);               //Erode with N Particles
  void grow();
};


void Terrain::erode(int cycles){

  //Track the Movement of all Particles
  bool track[dim.x*dim.y] = {false};

  //Do a series of iterations!
  for(int i = 0; i < cycles; i++){

    //Spawn New Particle
    vec2 newpos = glm::vec2(rand()%(int)dim.x, rand()%(int)dim.y);
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
  for(size_t i = 0; i < trees.size(); i++){

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


//Generate the terrain
void Terrain::generate(int ncycles){

  //Initialize
  logg::out("Initializing Terrain");
  srand(SEED);

  logg::out("Seed: ", SEED);

  //Perlin Noise Initial Terrain
  perlin(SEED, D);

  //Do a number of cycles...
  logg::out("Executing Erosion and Growth Steps");
  for(int i = 0; i < ncycles; i++){
    erode(250);
    grow();
    logg::progress(i, ncycles);
  }

  //Generate the World from this!
  logg::out("Processing Height and Pool Data");

  for(int x = 0; x < rescale.x*dim.x; x++){         //Rescale Terrain Dimension to Full Grid Resolution
    for(int y = 0; y < rescale.z*dim.y; y++){

      int i = x/(int)rescale.x;                     //Coordinates in Terrain Space
      int k = y/(int)rescale.z;

      double height, pool;                          //Interpolated Values
      float sx = (float)(x%rescale.x)/rescale.x;    //Remainder in World-Space
      float sy = (float)(y%rescale.z)/rescale.z;

      if(i < dim.x && k < dim.y){                   //Check Boundaries
        height =  (1.0-sy)*(1.0-sx)*rescale.y*scale*heightmap[i*dim.y+k];
        height += (1.0-sy)*(sx)*rescale.y*scale*heightmap[(i+1)*dim.y+k];
        height += (sy)*(1.0-sx)*rescale.y*scale*heightmap[i*dim.y+k+1];
        height += (sy)*(sx)*rescale.y*scale*heightmap[(i+1)*dim.y+k+1];
        pool =  (1.0-sy)*(1.0-sx)*rescale.y*scale*waterpool[i*dim.y+k];
        pool += (1.0-sy)*(sx)*rescale.y*scale*waterpool[(i+1)*dim.y+k];
        pool += (sy)*(1.0-sx)*rescale.y*scale*waterpool[i*dim.y+k+1];
        pool += (sy)*(sx)*rescale.y*scale*waterpool[(i+1)*dim.y+k+1];
      }
      else{
        height = rescale.y*scale*heightmap[i*dim.y+k];
        pool = rescale.y*scale*waterpool[i*dim.y+k];
      }

      bool _pool = (pool > 0.0);                    //Whether Pool Exists

      //Get the Surface Normal
      vec3 n = surfaceNormal(i*dim.y+k, heightmap, dim, scale);
      bool rock = (n.y < steepness);

      //Below Surface
      for(int j = 0; j < (int)height-3; j++){
        if(rock) blueprint.add(vec3(x, j, y), BLOCK_STONE, false);
        else blueprint.add(vec3(x, j, y), BLOCK_DIRT, false);
      }

      //Surface
      if(rock) blueprint.add(vec3(x, (int)height-1, y), BLOCK_STONE, false);
      else if(!_pool) blueprint.add(vec3(x, (int)height-1, y), BLOCK_GRASS, false);

      if(rock) blueprint.add(vec3(x, (int)height-2, y), BLOCK_STONE, false);
      else if(!_pool) blueprint.add(vec3(x, (int)height-2, y), BLOCK_GRASS, false);

      if(rock) blueprint.add(vec3(x, (int)height-3, y), BLOCK_STONE, false);
      else if(!_pool) blueprint.add(vec3(x, (int)height-3, y), BLOCK_GRASS, false);

      //Water on Surface
      if(_pool){
        blueprint.add(vec3(x, (int)height-3, y), BLOCK_GRAVEL, false);
        blueprint.add(vec3(x, (int)height-4, y), BLOCK_GRAVEL, false);
        for(int j = (int)height-2; j < (int)(height+pool); j++)
          blueprint.add(vec3(x, j, y), BLOCK_WATER, false);
      }
    }

    if(blueprint.edits.size() > blueprint::maxedits)
      blueprint::evaluate(blueprint);

  }

  blueprint::evaluate(blueprint);

  logg::out("Processing Stream Data");
  for(int x = 0; x < rescale.x*dim.x; x++){
    for(int y = 0; y < rescale.z*dim.y; y++){

      int i = x/(int)rescale.x;
      int k = y/(int)rescale.z;

      //Interpolate Values
      double height, stream;
      float sx = (float)(x%rescale.x)/rescale.x;
      float sy = (float)(y%rescale.z)/rescale.z;
      if(i < dim.x && k < dim.y){
        height =  (1.0-sy)*(1.0-sx)*rescale.y*scale*heightmap[i*dim.y+k];
        height += (1.0-sy)*(sx)*rescale.y*scale*heightmap[(i+1)*dim.y+k];
        height += (sy)*(1.0-sx)*rescale.y*scale*heightmap[i*dim.y+k+1];
        height += (sy)*(sx)*rescale.y*scale*heightmap[(i+1)*dim.y+k+1];
        stream =  (1.0-sy)*(1.0-sx)*waterpath[i*dim.y+k];
        stream += (1.0-sy)*(sx)*waterpath[(i+1)*dim.y+k];
        stream += (sy)*(1.0-sx)*waterpath[i*dim.y+k+1];
        stream += (sy)*(sx)*waterpath[(i+1)*dim.y+k+1];
      }
      else{
        height = rescale.y*scale*heightmap[i*dim.y+k];
        stream = waterpath[i*dim.y+k];
      }

      if(ease::langmuir(stream, 5.0) > 0.4){
        if(ease::langmuir(stream, 5.0) > 0.6){
          blueprint.add(vec3(x, (int)height-1, y), BLOCK_AIR, false);
          blueprint.add(vec3(x, (int)height-2, y), BLOCK_WATER, false);
          blueprint.add(vec3(x, (int)height-3, y), BLOCK_WATER, false);
          blueprint.add(vec3(x, (int)height-4, y), BLOCK_WATER, false);
          blueprint.add(vec3(x, (int)height-5, y), BLOCK_WATER, false);
          blueprint.add(vec3(x, (int)height-6, y), BLOCK_GRAVEL, false);
          blueprint.add(vec3(x, (int)height-7, y), BLOCK_GRAVEL, false);
          blueprint.add(vec3(x, (int)height-8, y), BLOCK_GRAVEL, false);
          blueprint.add(vec3(x, (int)height-9, y), BLOCK_GRAVEL, false);
        }
        else{
          blueprint.add(vec3(x, (int)height-1, y), BLOCK_GRAVEL, false);
          blueprint.add(vec3(x, (int)height-2, y), BLOCK_GRAVEL, false);
          blueprint.add(vec3(x, (int)height-3, y), BLOCK_GRAVEL, false);
          blueprint.add(vec3(x, (int)height-4, y), BLOCK_GRAVEL, false);
        }
      }

    }
  }


  blueprint::evaluate(blueprint);


  logg::out("Processing Tree Data");

  //Add trees according to the tree number and density
  for(unsigned int n = 0; n < trees.size()*rescale.x*rescale.z; n++){

    int p[2] = {rand()%(int)(rescale.x*dim.x), rand()%(int)(rescale.z*dim.y)};
    int i = p[0]/(int)rescale.x;
    int k = p[1]/(int)rescale.z;

    //Check for Tree Density
    if((float)(rand()%1000)/1000.0 > plantdensity[i*dim.y+k]) continue;

    //Interpolate Values
    double height;
    float sx = (float)(p[0]%rescale.x)/rescale.x;
    float sy = (float)(p[1]%rescale.z)/rescale.z;
    if(i < dim.x && k < dim.y){
      height =  (1.0-sy)*(1.0-sx)*rescale.y*scale*heightmap[i*dim.y+k];
      height += (1.0-sy)*(sx)*rescale.y*scale*heightmap[(i+1)*dim.y+k];
      height += (sy)*(1.0-sx)*rescale.y*scale*heightmap[i*dim.y+k+1];
      height += (sy)*(sx)*rescale.y*scale*heightmap[(i+1)*dim.y+k+1];
    }
    else height = rescale.y*scale*heightmap[i*dim.y+k];

    blueprint += (blueprint::tree(rand()%6+4) + vec3(p[0], (int)height, p[1]));

  //  delete _tree;

  }

  blueprint::evaluate(blueprint);

}



void Terrain::perlin(int SEED, vec3 d){

  noise::module::Perlin perlin;
  perlin.SetOctaveCount(8);
  perlin.SetFrequency(1.0);
  perlin.SetPersistence(0.5);

  double min = 0.0, max = 0.0;
  for(int i = 0; i < d.x*d.z; i++){
    heightmap[i] = perlin.GetValue((i/(int)d.z)*(1.0/d.x), (i%(int)d.z)*(1.0/d.z), SEED);
    if(heightmap[i] > max) max = heightmap[i];
    if(heightmap[i] < min) min = heightmap[i];
  }

  for(int i = 0; i < d.x*d.z; i++){
    heightmap[i] = (heightmap[i] - min)/(max - min);
  }

}


void Terrain::perlin(int SEED, vec3 d, bool(World::*eval)(Blueprint& print), World* w){
//void Terrain::perlin(int SEED, vec3 d){

  noise::module::Perlin perlin;
  perlin.SetOctaveCount(8);
  perlin.SetFrequency(1.0);
  perlin.SetPersistence(0.5);

  double min = 0.0, max = 0.0;
  for(int i = 0; i < d.x*d.z; i++){
    heightmap[i] = perlin.GetValue((i/(int)d.z)*(1.0/d.x), (i%(int)d.z)*(1.0/d.z), SEED);
    if(heightmap[i] > max) max = heightmap[i];
    if(heightmap[i] < min) min = heightmap[i];
  }

  for(int i = 0; i < d.x*d.z; i++){
    heightmap[i] = d.y*(heightmap[i] - min)/(max - min);
  }

  for(int i = 0; i < d.x; i++){
    for(int j = 0; j < d.z; j++){
      int h = heightmap[i*(int)d.z+j];
      blueprint.add(vec3(i, h, j), BLOCK_GRASS, false);
      for(int k = h-1; k >=0; k--)
        blueprint.add(vec3(i, k, j), BLOCK_STONE, false);
    }

    if(blueprint.edits.size() > blueprint::maxedits)
      blueprint::evaluate(blueprint);

  }

  blueprint::evaluate(blueprint);

}


#endif
