#pragma once
#include "../forward/chunk.fwd.h"
#include "../forward/octree.fwd.h"

class Octree{
public:
  //Needs to be set initially.
  BlockType type = BLOCK_AIR;
  std::vector<Octree> subTree;  //Sparse SubTree
  unsigned char depth = 4;                    //Current Depth
  unsigned char index = 0;                    //Current Index

  //Back and forth to chunks
  void fromChunk(Chunk &chunk);

  //Filling and Combining Operations for Subtrees
  bool trySimplify();
  bool contains(glm::vec3 _pos);

  //Index Functions
  glm::vec3 getPos(int index);
  int getIndex(glm::ivec3 _pos);

  //We need setters, getters, volume setters, etc.
  bool setPosition(glm::vec3 _pos, BlockType _type);
  BlockType getPosition(glm::vec3 _pos, int LOD);
};

//IO Operator for Octree (Debugging)
std::ostream & operator << (std::ostream &out, const Octree &octree){
  //Output Depth and Index
  out<<"["<<octree.depth<<", "<<octree.index<<"]: ";

  //Output the Data
  out<<"{"<<(int)octree.type<<"}"<<std::endl;

  //Output the Subtree
  for(int i = 0; i < octree.subTree.size(); i++){
    for(int j = 0; j <= 4-octree.depth; j++)
      out<<" ";
    out<<octree.subTree[i];
  }

  return out;
}

/*
  Right Now:

  It will store 8 subtrees if it isn't air, and there is something else inside.
  Does this make sense?
  Not really.

  It really should be that the subtree by default adopts whatever the supertree's value is.

  The supertree always takes the majority element for compression reasons...
*/
