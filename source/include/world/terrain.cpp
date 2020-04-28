/*
    These Functions are all relevant to the world generation.
*/

#include "terrain.h"

//Generate the terrain
void Terrain::generate(){
  //Initialize
  logger::write("Initializing Terrain");
  if(w->SEED == 0) w->SEED = time(NULL);
  srand(w->SEED);

  logger::write("Seed: ", w->SEED);

  //Perlin Noise Initial Terrain
  perlin();

  //Do a number of cycles...
  logger::write("Executing Erosion and Growth Steps");
  int ncycles = 1500;
  for(int i = 0; i < ncycles; i++){
    erode(250);
    grow();
    logger::progress(i, ncycles);
  }

  //Generate the World from this!
  logger::write("Processing Height and Pool Data");
  for(int x = 0; x < rescale.x*dim.x; x++){
    for(int y = 0; y < rescale.z*dim.y; y++){

      int i = x/(int)rescale.x;
      int k = y/(int)rescale.z;

      //Interpolate Values
      double height, pool;
      float sx = (float)(x%rescale.x)/rescale.x;
      float sy = (float)(y%rescale.z)/rescale.z;
      if(i < dim.x && i < dim.y){
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

      bool _pool = (pool > 0.0);

      //Get the Surface Normal
      glm::vec3 n = surfaceNormal(i*dim.y+k, heightmap, dim, scale);
      bool rock = (n.y < steepness);

      //Below Surface
      for(int j = 0; j < (int)height-1; j++){
        if(rock) w->blueprint.addEditBuffer(glm::vec3(x, j, y), BLOCK_STONE, false);
        else w->blueprint.addEditBuffer(glm::vec3(x, j, y), BLOCK_DIRT, false);
      }

      //Surface
      if(rock) w->blueprint.addEditBuffer(glm::vec3(x, (int)height-1, y), BLOCK_STONE, false);
      else if(!_pool) w->blueprint.addEditBuffer(glm::vec3(x, (int)height-1, y), BLOCK_GRASS, false);

      //Water on Surface
      if(_pool){
        w->blueprint.addEditBuffer(glm::vec3(x, (int)height-2, y), BLOCK_GRAVEL, false);
        for(int j = (int)height-1; j < (int)(height+pool); j++)
          w->blueprint.addEditBuffer(glm::vec3(x, j, y), BLOCK_WATER, false);
      }
    }
  }
  w->evaluateBlueprint(w->blueprint);

  logger::write("Processing Stream Data");
  for(int x = 0; x < rescale.x*dim.x; x++){
    for(int y = 0; y < rescale.z*dim.y; y++){

      int i = x/(int)rescale.x;
      int k = y/(int)rescale.z;

      //Interpolate Values
      double height, stream;
      float sx = (float)(x%rescale.x)/rescale.x;
      float sy = (float)(y%rescale.z)/rescale.z;
      if(i < dim.x && i < dim.y){
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
          w->blueprint.addEditBuffer(glm::vec3(x, (int)height-1, y), BLOCK_AIR, false);
          w->blueprint.addEditBuffer(glm::vec3(x, (int)height-2, y), BLOCK_WATER, false);
          w->blueprint.addEditBuffer(glm::vec3(x, (int)height-3, y), BLOCK_GRAVEL, false);
        }
        else w->blueprint.addEditBuffer(glm::vec3(x, (int)height-1, y), BLOCK_GRAVEL, false);
      }

    }
  }
  w->evaluateBlueprint(w->blueprint);

  logger::write("Processing Tree Data");
  Blueprint _tree;

  //Add trees according to the tree number and density
  for(unsigned int n = 0; n < trees.size()*rescale.x*rescale.z; n++){

    int p[2] = {rand()%(int)(rescale.x*dim.x), rand()%(int)(rescale.z*dim.y)};
    int i = p[0]/(int)rescale.x;
    int k = p[1]/(int)rescale.z;

    //Check for Tree Density
    if((float)(rand()%1000)/1000.0 > plantdensity[i*dim.y+k]) continue;

    _tree.editBuffer.clear();
    _tree.tree(rand()%6+4);

    //Interpolate Values
    double height;
    float sx = (float)(p[0]%rescale.x)/rescale.x;
    float sy = (float)(p[1]%rescale.z)/rescale.z;
    if(i < dim.x && i < dim.y){
      height =  (1.0-sy)*(1.0-sx)*rescale.y*scale*heightmap[i*dim.y+k];
      height += (1.0-sy)*(sx)*rescale.y*scale*heightmap[(i+1)*dim.y+k];
      height += (sy)*(1.0-sx)*rescale.y*scale*heightmap[i*dim.y+k+1];
      height += (sy)*(sx)*rescale.y*scale*heightmap[(i+1)*dim.y+k+1];
    }
    else height = rescale.y*scale*heightmap[i*dim.y+k];

    w->blueprint.merge(_tree.translate(glm::vec3(p[0], (int)height, p[1])));

  }

  w->evaluateBlueprint(w->blueprint);
}

void Terrain::perlin(){
  noise::module::Perlin perlin;
  perlin.SetOctaveCount(8);
  perlin.SetFrequency(1.0);
  perlin.SetPersistence(0.5);

  double min, max = 0.0;
  for(int i = 0; i < dim.x*dim.y; i++){
    heightmap[i] = perlin.GetValue((i/dim.y)*(1.0/dim.x), (i%dim.y)*(1.0/dim.y), w->SEED);
    if(heightmap[i] > max) max = heightmap[i];
    if(heightmap[i] < min) min = heightmap[i];
  }

  for(int i = 0; i < dim.x*dim.y; i++)
    heightmap[i] = (heightmap[i] - min)/(max - min);

}

void Terrain::flat(){
  w->blueprint.flatSurface(rescale.x*dim.x, rescale.z*dim.y);
  w->evaluateBlueprint(w->blueprint);
}


/*
void World::generateBuildings(){
  //Flat Surface

  //Add some Cacti
  Blueprint _cactus;
  for(int i = 0; i < 500; i++){
    _cactus.cactus();
    blueprint.merge(_cactus.translate(glm::vec3(rand()%(int)(dim.x*chunkSize), 1, rand()%(int)(dim.z*chunkSize))));
  }
  evaluateBlueprint(blueprint);

  //Add Generated Buildings
  Blueprint _building;
  _building.building<RUSTIC>(5);  //Recommended Max-Size: 5 (can handle 6)
  blueprint.merge(_building.translate(glm::vec3(100, 1, 100)));
  evaluateBlueprint(blueprint);
}
*/
