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
  //Output the Current dive Depth
  //If we are at a node, return true
  if(subTree.empty()){
    return true;
  }

  //Types of all blocks
  BlockType _type[subTree.size()];
  bool fail = false;
  //Loop over all elements
  for(unsigned int i = 0; i < subTree.size(); i ++){
    //Check if subelement is simple!
    if(!subTree[i].trySimplify()){
      //We have a non-simple subelement -> can't combine
      fail = true;
    }
    //Add the subtree's element to the type list! (even for failed)
    _type[i] = subTree[i].type;
  }

  if(subTree.size()!=8){
    fail = true;
  }

  //All sub-elements are simple, so check if they are identical
  if(!fail){
    //Loop over the guys to compare
    for(unsigned int i = 1; i < subTree.size(); i++){
      //If they are not all identical!
      if(_type[i] != _type[0]){
        fail = true;
      }
    }
  }

  //Sub-elements are either non-simple, or non-identical. Set Majority
  if(fail){
    //By default it's air
    int Maj = 0;
    BlockType majType = BLOCK_AIR;
    //Find the Majority Class
    for(unsigned int i = 0; i < subTree.size(); i++){
      //Make sure we don't observe air
      if(_type[i] != BLOCK_AIR){
        int newMaj = std::count(_type, _type+subTree.size(), _type[i]);
        //If we have more of that specific type, make sure we update maj
        if(newMaj > Maj){
          Maj = newMaj;
          majType = _type[i];
        }
      }
    }

    //Set the Majority Class
    type = majType;
    return false;
  }

  //We still haven't failed, so combine them and
  if(!fail){
    //Find the Majority Class

    //One of our subelements is non-simple or non-matching, so we can't combine
    subTree.clear();
    type = _type[0];
    return true;
  }

  //We failed, so return false.
  return false;
}

bool Octree::contains(glm::vec3 _pos){
  //This only checks if the x, y, z coordinates are within the width of the octree element
  int width = pow(2,depth);

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
  int width = pow(2,depth-1);
  glm::vec3 _p = glm::floor(_pos/glm::vec3(width));
  std::byte _index = getIndex(_p);

  //Loop over our subTree, see if we can find the node with the right index.
  for(unsigned int i = 0; i < subTree.size(); i++){
    //If we have the node at the right position...
    if(subTree[i].index == _index){
      //Set the position in that node.
      return subTree[i].setPosition(_pos - _p*glm::vec3(width), _type);
    }
  }

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
  //Check if we are at the bottom of the line
  if(subTree.empty() || LOD == 0){
    return type;
  }
  //Find the subtree element that contains the value
  for(unsigned int i = 0; i < subTree.size(); i++){
    //Binary Representation in 3 Coords
    glm::vec3 p = getPos(subTree[i].index);
    int width = pow(2, depth-1);

    //Check if the subtree contains it
    if(subTree[i].contains(_pos - p*glm::vec3(width))){
      //We found the element that we wish to try to replace now.
      return subTree[i].getPosition(_pos - p*glm::vec3(width), LOD-1);
    }
  }
  return BLOCK_AIR;
}

glm::vec3 Octree::getPos(std::byte index){
  //(XYZ) = (111)
  //Generate Binary Sequence
  int _x = (int)(index)/4;
  int _y = ((int)index-4*_x)/2;
  int _z = ((int)index-4*_x - 2*_y);
  //Return Binary Vectory
  return glm::vec3(_x, _y, _z);
}

std::byte Octree::getIndex(glm::vec3 _pos){
  //Decode the Binary into Decimals
  return (std::byte)(_pos.x*4+_pos.y*2+_pos.z*1);
}
