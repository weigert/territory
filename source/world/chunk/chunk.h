#ifndef TERRITORY_CHUNK
#define TERRITORY_CHUNK

#define CHUNKSIZE 16
#define CHUNKVOL CHUNKSIZE*CHUNKSIZE*CHUNKSIZE

enum BiomeType{
  BIOME_VOID = 0,
  BIOME_DESERT = 1,
  BIOME_FOREST = 2,
};

using namespace glm;

#include "block.h"

class Chunk{
public:

  Chunk(){
    data = new unsigned char[CHUNKVOL];
    for(int i = 0; i < CHUNKVOL; i++) data[i] = 0;
  }

  Chunk(vec3 _pos, BiomeType type):Chunk(){
    pos = _pos;// biome = type;
  }

  ~Chunk(){
  //  if(data != NULL) delete[] data;
  }

  //Position information and CHUNKSIZE information
  ivec3 pos = ivec3(0);
//  BiomeType biome;

  unsigned char* data = NULL;

  bool remesh = true;

  int quadsize = 0;
  int quadstart = 0;

  //Get the Flat-Array Index
  int getIndex(vec3 _p);
  void setPosition(vec3 _p, BlockType _type);
  BlockType getPosition(vec3 _p);
  BlockType getPosition(int* p);

};

bool operator > (const Chunk& a, const Chunk& b) {

  if(a.pos.x > b.pos.x) return true;    //Sort by Position
  if(a.pos.x < b.pos.x) return false;

  if(a.pos.y > b.pos.y) return true;    //Sort by Position
  if(a.pos.y < b.pos.y) return false;

  if(a.pos.z > b.pos.z) return true;    //Sort by Position
  if(a.pos.z < b.pos.z) return false;

  return false;

}

int Chunk::getIndex(vec3 _p){
  //Return the Correct Index
  if(all(lessThan(_p, vec3(CHUNKSIZE)))){
    return _p.x*CHUNKSIZE*CHUNKSIZE+_p.y*CHUNKSIZE+_p.z;
  }
  return 0;
}

void Chunk::setPosition(vec3 _p, BlockType _type){
  data[getIndex(_p)] = (int)_type;
}

BlockType Chunk::getPosition(vec3 _p){
  return (BlockType)data[getIndex(_p)];
}

BlockType Chunk::getPosition(int* p){
  return (BlockType)data[p[0]*CHUNKSIZE*CHUNKSIZE+p[1]*CHUNKSIZE+p[2]];
}

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, ivec3 & _vec, const unsigned int version)
{
  ar & _vec.x;
  ar & _vec.y;
  ar & _vec.z;
}

template<class Archive>
void serialize(Archive & ar, Chunk & _chunk, const unsigned int version)
{
  ar & _chunk.pos;
  ar & _chunk.data;
}

}
}

#endif
