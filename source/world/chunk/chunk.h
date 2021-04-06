#ifndef TERRITORY_CHUNK
#define TERRITORY_CHUNK

using namespace glm;

#include "block.h"

class Chunk{
public:

  Chunk(){}

  Chunk(vec3 _pos, BlockType* ndata){
    pos = _pos;
    data = ndata;
  }

  ivec3 pos = ivec3(0);
  BlockType* data;


  BlockType* neighbor[6]{NULL};




  bool remesh = true;
  bool firstmesh = true;

  int quadsize = 0;
  int quadstart = 0;
  array<uint*, 6> faces;

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

bool operator < (const Chunk& a, const Chunk& b) {

  if(a.pos.x < b.pos.x) return true;    //Sort by Position
  if(a.pos.x > b.pos.x) return false;

  if(a.pos.y < b.pos.y) return true;    //Sort by Position
  if(a.pos.y > b.pos.y) return false;

  if(a.pos.z < b.pos.z) return true;    //Sort by Position
  if(a.pos.z > b.pos.z) return false;

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
  data[getIndex(_p)] = _type;
}

BlockType Chunk::getPosition(vec3 _p){
  return data[getIndex(_p)];
}

BlockType Chunk::getPosition(int* p){
  return data[p[0]*CHUNKSIZE*CHUNKSIZE+p[1]*CHUNKSIZE+p[2]];
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
