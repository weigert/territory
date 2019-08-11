#include "octree.h"
#include <bits/stdc++.h>

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

bool Octree::contains(int x, int y, int z){
  //This only checks if the x, y, z coordinates are within the width of the octree element
  int width = pow(2,depth);
  if(x < width && y < width && z < width && x >= 0 && y >= 0 && z >= 0){
    return true;
  }
  return false;
}

bool Octree::setPosition(int x, int y, int z, BlockType _type){
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
  glm::vec3 _p = glm::vec3(x/width, y/width, z/width);
  int _index = getIndex(_p);

  //Loop over our subTree, see if we can find the node with the right index.
  for(unsigned int i = 0; i < subTree.size(); i++){
    //If we have the node at the right position...
    if(subTree[i].index == _index){
      //Set the position in that node.
      return subTree[i].setPosition(x - _p.x*width, y - _p.y*width, z - _p.z*width, _type);
    }
  }

  //Finally, we haven't found the right subTree element. Add an element to the subTree, and expand it.
  Octree element;
  element.depth = depth-1;
  element.type = type;
  element.index = _index;
  element.setPosition(x - _p.x*width, y - _p.y*width, z - _p.z*width, _type);
  subTree.push_back(element);

  return true;
}

BlockType Octree::getPosition(int x, int y, int z, int LOD){
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
    if(subTree[i].contains(x - p.x*width, y - p.y*width, z - p.z*width)){
      //We found the element that we wish to try to replace now.
      return subTree[i].getPosition(x-p.x*width, y-p.y*width, z-p.z*width, LOD-1);
    }
  }
  return BLOCK_AIR;
}

glm::vec3 Octree::getPos(int index){
  //(XYZ) = (111)
  //Generate Binary Sequence
  int _x = index/4;
  int _y = (index-4*_x)/2;
  int _z = (index-4*_x - 2*_y);
  //Return Binary Vectory
  return glm::vec3(_x, _y, _z);
}

int Octree::getIndex(glm::vec3 pos){
  //Decode the Binary into Decimals
  return pos.x*4+pos.y*2+pos.z*1;
}

glm::vec4 Octree::getColorByID(BlockType _type){
  //Switch the value and return a vector
  glm::vec4 color;
  switch(_type){
    case BLOCK_GRASS:
      color = glm::vec4(0.54f, 0.7f, 0.34f, 1.0f);
      break;
    case BLOCK_DIRT:
      color = glm::vec4(0.74f, 0.5f, 0.36f, 1.0f);
      break;
    case BLOCK_WATER:
      color = glm::vec4(0.02f, 0.61f, 0.75f, 1.0f);
      break;
    case BLOCK_SAND:
      color = glm::vec4(0.93f, 0.91f, 0.38f, 1.0f);
      break;
    case BLOCK_CLAY:
      color = glm::vec4(0.97f, 0.5f, 0.44f, 1.0f);
      break;
    case BLOCK_STONE:
      color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
      break;
    case BLOCK_LEAVES:
      color = glm::vec4(0.38f, 0.48f, 0.26f, 0.8f);
      break;
    case BLOCK_WOOD:
      color = glm::vec4(0.6f, 0.375f, 0.14f, 1.0f);
      break;
    default:
      color = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
      break;
  }
  return color;
}
