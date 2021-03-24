#ifndef TERRITORY_WORLD
#define TERRITORY_WORLD

using namespace std;
using namespace glm;

class World;

#include "chunk/chunk.h"
#include "blueprint/blueprint.cpp"
#include "chunk/mesh.h"

#include "terrain/terrain.h"

class World{
public:

World(string savefile, int _SEED){
  SEED = _SEED;
  dimr = min(dim, dimr); //Edge Case

  bufferObject::rdim = dimr;

  if(!init(savefile)){
    logg::err("Couldn't Initialize World");
  }
  logg::deb("Initialized World");
}

int SEED = 0;                         //World Seed
vec3 dim = vec3(64, 16, 64);          //Spatial Information (Size in Chunks)
vec3 dimr = vec3(16, 16, 16);         //Chunks Per Region
int chunkSize = CHUNKSIZE;

vec3 minchunk = vec3(0);
vec3 maxchunk = vec3(0);

Blueprint blueprint;
Blueprint remeshbuf;
vector<Chunk> chunks;                 //Loaded Chunks

int sealevel = 16;

vector<Model*> models;
Model fullmodel;
void mesh();
void remesh();

volatile int time = 0;  //Is set in a separate timed thread.
chrono::milliseconds tickLength = chrono::milliseconds(100);

bool init(std::string savefile);

void blank();
bool evaluateBlueprint(Blueprint* print);
bool evaluateBlueprint(Blueprint& print);
string regionString(vec3 cpos);

bool lock = false;
void buffer();

};

namespace boost {
namespace serialization {

//World Serializer
template<class Archive>
void serialize(Archive & ar, World & _world, const unsigned int version)
{
  ar & _world.chunkSize;
  ar & _world.dim.x;
  ar & _world.dim.y;
  ar & _world.dim.z;
}

}
}

#endif
