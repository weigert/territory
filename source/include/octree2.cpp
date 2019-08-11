#include "octree2.h"
#include <bits/stdc++.h>

/*
bool Octree::trySimplify(){
  //Output the Current dive Depth
  //If we are at a node, return true
  if(subTree.empty()){
    return true;
  }
  else{
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
}*/


bool Octree::trySimplify(){
  //Output the Current dive Depth
  //If we are at a node, return true
  if(subTree.empty()){
    return true;
  }
  else{
    //Types of all blocks
    BlockType _type[8];
    bool fail = false;
    //Loop over all elements
    for(int i = 0; i < 8; i ++){
      //Check if subelement is simple!
      if(!subTree[i].trySimplify()){
        //We have a non-simple subelement -> can't combine
        fail = true;
      }
      //Add the subtree's element to the type list! (even for failed)
      _type[i] = subTree[i].type;
    }

    //All sub-elements are simple, so check if they are identical
    if(!fail){
      //Loop over the guys to compare
      for(int i = 1; i < 8; i++){
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
      for(int i = 0; i < 8; i++){
        //Make sure we don't observe air
        if(_type[i] != BLOCK_AIR){
          int newMaj = std::count(_type, _type+8, _type[i]);
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
    std::cout<<"ayy"<<std::endl;
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
    //Not at final node, not identical, subtree empty
    else{
        //For 8 times, we construct a new octree element and add it in.
        Octree element;
        //Depth is one less now
        element.depth = depth-1;
        element.type = type;

        //Why do we have to add 8 elements to the subtree, not less?
        for(int i = 0; i < 8; i++){
          element.index = i;
          subTree.push_back(element);
        }
    }
  }
  //Shorten the x,y,z and try to setposition
  for(unsigned int i = 0; i < subTree.size(); i++){
    //Moving Coordinates (i.e. binary conversion)
    glm::vec3 p = getPos(subTree[i].index);
    int width = pow(2,depth-1);
    //Check if a subtree element contains the element
    if(subTree[i].contains(x - p.x*width, y - p.y*width, z - p.z*width)){
      std::cout<<subTree[i].index<<std::endl;
      //We found the element that we wish to try to replace now.
      return subTree[i].setPosition(x - p.x*width, y-p.y*width, z-p.z*width, _type);
    }
  }
  return false;
}

BlockType Octree::getPosition(int x, int y, int z, int LOD){
  //Check if we are at the bottom of the line
  if(subTree.empty() || depth == 0 || LOD == 0){
    return type;
  }
  //Loop over the Subtree, see if any contain it.
  for(unsigned int i = 0; i < subTree.size(); i++){
    //Get the index of the guy in the subtree
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
