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

  tempchunk = new BlockType[CHUNKVOL]{BLOCK_AIR};

  if(!init(savefile))
    logg::err("Couldn't Initialize World");
  logg::deb("Initialized World");

  vertexpool.reserve(scene::QUAD, scene::MAXBUCKET);
  vertexpool.index();

  //Reserve Chunkpool Data
  const size_t Nchunks = 32*32*32*4;
  chunkpool = new BlockType[CHUNKVOL*Nchunks]{BLOCK_AIR}; //This needs deleting
  for(size_t i = 0; i < Nchunks; i++)
    free.push_front(chunkpool+i*CHUNKVOL);

}

~World(){
  delete[] chunkpool;
  delete[] tempchunk;
}

int SEED = 0;                         //World Seed
vec3 dim = WDIM;        //Spatial Information (Size in Chunks)
vec3 dimr = RDIM;         //Chunks Per Region
int chunkSize = CHUNKSIZE;

ivec3 minchunk = ivec3(-1);
ivec3 maxchunk = ivec3(-1);




BlockType* chunkpool;
BlockType* tempchunk;
deque<BlockType*> free;



BlockType* getChunkPointer(){
  if(free.empty()) std::cout<<"CAN'T ALLOCATE CHUNK: OUT OF MEMORY"<<std::endl;
  BlockType* freechunk = free.back();
  free.pop_back();
  return freechunk;
}

void returnChunkPoiner(BlockType* freechunk){
  free.push_front(freechunk);
}





Blueprint blueprint;
Blueprint remeshbuf;
vector<Chunk> chunks;                 //Loaded Chunks
Vertexpool<Vertex> vertexpool;




int sealevel = 16;

void mesh();

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
