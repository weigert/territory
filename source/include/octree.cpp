#include "octree.h"

void Octree::setRoot(){
  isRoot = true;
  depth = 0;
}

void Octree::fillSubTree(){
  isNode = false;
  //Check that the depth is fine
  if(depth < maxDepth){
    //For 8 times, we construct a new octree element and add it in.
    Octree element;
    element.depth = depth+1;
    element.type = type;
    element.maxDepth = maxDepth;
    for(int i = 0; i < 8; i ++){
      subTree.push_back(element);
    }
  }
}

bool Octree::trySimplify(){
  //If we are at a node, return true
  if(subTree.empty()){
    std::cout<<"We are at node. No simplification possible."<<std::endl;
    return true;
  }
  else{
    //Variable to see if all subelements are identical
    BlockType _type = subTree[0].type; //First element's value
    //Loop over all elements
    for(int i = 0; i < 8; i ++){
      //Check if subelement is simple!
      if(!subTree[i].trySimplify()){
        //We have a non-simple subelement -> can't combine
        return false;
      }
      //We have a simple subelement
      if(subTree[i].type != _type){
        //Not all elements are identical
        return false;
      }
    }
    //We have all simple subelements, and they are identical
    subTree.clear();
    type = _type;
    isNode = true;
    std::cout<<"Node simplified"<<std::endl;
    return true;
  }
}

bool Octree::contains(int x, int y, int z){
  //This only checks if the x, y, z coordinates are within the width of the octree element
  int width = pow(2,(maxDepth-depth));
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
    else if(depth == maxDepth){
      //Set the element and return true
      type = _type;
      return true;
    }
    //Not at final node, not identical, subtree empty
    else{
      //Fill the subtree so we can expand
      fillSubTree();
    }
  }
  //Shorten the x,y,z and try to setposition
  for(int i = 0; i < 8; i++){
    //Moving Coordinates (i.e. binary conversion)
    int _x = i/4;
    int _y = (i - 4*_x)/2;
    int _z = (i - 4*_x - 2*_y);
    int width = pow(2,maxDepth-depth-1);
    //Check if a subtree element contains the element
    if(subTree[i].contains(x - _x*width, y - _y*width, z - _z*width)){
      //We found the element that we wish to try to replace now.
      return subTree[i].setPosition(x - _x*width, y-_y*width, z-_z*width, _type);
    }
  }
  std::cout<<"Failure setting Octree element."<<std::endl;
  return false;
}

BlockType Octree::getPosition(int x, int y, int z){
  //Check if we are at the bottom of the line
  if(subTree.empty() || depth == maxDepth){
    return type;
  }
  //Find the subtree element that contains the value
  for(int i = 0; i < 8; i++){
    //Binary Representation in 3 Coords
    int _x = i/4;
    int _y = (i - 4*_x)/2;
    int _z = (i - 4*_x - 2*_y);
    int width = pow(2, maxDepth-depth-1);

    //Check if the subtree contains it
    if(subTree[i].contains(x - _x*width, y - _y*width, z - _z*width)){
      //We found the element that we wish to try to replace now.
      return subTree[i].getPosition(x-_x*width, y-_y*width, z-_z*width);
    }
  }
  std::cout<<"Failure getting Octree element."<<std::endl;
  return BLOCK_AIR;
}
