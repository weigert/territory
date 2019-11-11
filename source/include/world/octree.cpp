#include "chunk.h"
#include "octree.h"
#include <bits/stdc++.h>


//Generate Octree from Chunks
void Octree::fromChunk(Chunk &chunk){
  pos = chunk.pos;
  //Write the Chunk
  for(int i = 0; i < chunk.size; i++){
    for(int j = 0; j < chunk.size; j++){
      for(int k = 0; k < chunk.size; k++){
        setPosition(glm::vec3(i, j, k), (BlockType)chunk.data[chunk.getIndex(glm::vec3(i,j,k))]);
      }
    }
  }
  //Simplify the Octree
  trySimplify();
}

Chunk Octree::toChunk(){
  Chunk chunk;
  chunk.pos = pos;
  //Do the thing
  for(int i = 0; i < chunk.size; i++){
    for(int j = 0; j < chunk.size; j++){
      for(int k = 0; k < chunk.size; k++){
        chunk.data[chunk.getIndex(glm::vec3(i,j,k))] = (int)getPosition(glm::vec3(i, j, k), depth);
      }
    }
  }
  return chunk;
}


bool Octree::trySimplify(){
  if(subTree.empty() || depth == 0) return true;

  //Types of all blocks
  BlockType _type[subTree.size()];
  bool success = true;

  //Loop over all elements
  for(unsigned int i = 0; i < subTree.size(); ++i){
    //Check if subelement is simple!
    if(!subTree[i].trySimplify()) success = false;
    else if(subTree[i].type == BLOCK_AIR) subTree.erase(subTree.begin()+i);

    //Add the subtree's element to the type list! (even for failed)
    _type[i] = subTree[i].type;
  }

  //Maybe we removed all subelements?
  if(subTree.empty()) return true;

  //Make sure we don't simplify non-complete subTrees.
  if(subTree.size()!=8 || !success) return false;

  //Loop over the guys to compare
  for(unsigned int i = 0; i < subTree.size(); i++){
    //Not all sub-components are identical - no simplification possible.
    if(_type[i] != _type[0]) return false;
  }

  //Clear the Subtree, set the type, return true.
  subTree.clear();
  type = _type[0];
  return true;
}

bool Octree::contains(glm::vec3 _pos){
  //This only checks if the x, y, z coordinates are within the width of the octree element
  int width = 1<<depth;
  return glm::all(glm::lessThan(_pos, glm::vec3(width))) && glm::all(glm::greaterThanEqual(_pos, glm::vec3(0)));
}

bool Octree::setPosition(glm::vec3 _pos, BlockType _type){
  //If we are at a node in the octree, that contains the element
  if(subTree.empty()){
    //Values are identical
    if(type == _type){
      //Simply return true
      return true;
    }
    //We are at the lowest depth
    else if(depth == 0){
      //Set the element and return true
      type = _type;
      return true;
    }
  }

  //Get the Position we want to set.
  int width = 1<<depth-1;
  glm::vec3 _p = glm::floor(_pos/glm::vec3(width));
  int _index = getIndex(_p);

  //Loop over our subTree, see if we can find the node with the right index, then set the appropriate position.
  for(auto &_subtree:subTree)
    if(_subtree.index == _index)
      return _subtree.setPosition(_pos - _p*glm::vec3(width), _type);

  //Finally, we haven't found the right subTree element. Add an element to the subTree, and expand it.
  Octree element;
  element.depth = depth-1;
  element.type = type;
  element.index = _index;
  element.setPosition(_pos - _p*glm::vec3(width), _type);
  subTree.push_back(element);

  return true;
}

BlockType Octree::getPosition(glm::vec3 _pos, int LOD){
  //Check if we are at the bottom of the line (in any sense)
  if(depth == 0 || subTree.empty() || LOD == 0) return type;

  //Check which SubTree contains the requested position
  for(auto &_subtree:subTree)
    if(_subtree.contains(_pos - getPos(_subtree.index)*glm::vec3(1<<depth-1)))
      return _subtree.getPosition(_pos - getPos(_subtree.index)*glm::vec3(1<<depth-1), LOD-1);

  //Didn't find the guy, therefore it's air.
  return BLOCK_AIR;
}

glm::vec3 Octree::getPos(int index){
  return glm::vec3((index>>2)%2, (index>>1)%2, (index>>0)%2);
}

int Octree::getIndex(glm::ivec3 _pos){
  return _pos.x*4+_pos.y*2+_pos.z*1;
}
