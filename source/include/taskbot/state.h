#pragma once
#include "../forward/state.fwd.h"
#include "../forward/world.fwd.h"
#include "../forward/population.fwd.h"
#include "../forward/bot.fwd.h"
#include "../forward/item.fwd.h"

class State{
  public:
    //Basic set of mandate requirements
    glm::vec3 pos = glm::vec3(0);
    glm::vec3 range = glm::vec3(0);
    std::string task = "Null";

    //Target Bot
    int target = 0;
    //int owner; //Owner of this Memory
    //int starter; //Originator of this Memory
    BlockType block = BLOCK_AIR;
    bool reachable = true;
    int time = 0;

    //States need to contain an inventory!
    Inventory inventory;

    //Get the Current Local State
    void retrieveState(World &world, Population &population, int bot);

    //Overloaded Operators
    void operator=(const State& rhs) {
      //Set all properties
      pos = rhs.pos;
      task = rhs.task;
      block = rhs.block;
      reachable = rhs.reachable;
      time = rhs.time;
      range = rhs.range;
    }
};


//Compare States, return a vector of keys?
bool operator==(State lhs, const State& rhs) {
  //Check all Properties
  if(lhs.pos != rhs.pos){
    return false;
  }
  if(lhs.task != rhs.task){
    return false;
  }
  if(lhs.block != rhs.block){
    return false;
  }
  if(lhs.reachable != rhs.reachable){
    return false;
  }
  if(lhs.time != rhs.time){
    return false;
  }
  if(lhs.range != rhs.range){
    return false;
  }
  return true;
}

//Compare States, return a vector of keys?
bool operator||(State lhs, const State& rhs) {
  //Check all Properties
  if(lhs.pos == rhs.pos){
    return true;
  }
  if(lhs.task == rhs.task){
    return true;
  }
  if(lhs.block == rhs.block){
    return true;
  }
  if(lhs.reachable == rhs.reachable){
    return true;
  }
  if(lhs.time == rhs.time){
    return true;
  }
  if(lhs.range == rhs.range){
    return true;
  }
  return false;
}

//Compare States, return a vector of keys?
State operator-(State lhs, const State& rhs) {
  //Construct new State
  State newState;
  //Continuous Values
  newState.pos = lhs.pos - rhs.pos;
  newState.time = lhs.time - rhs.time;
  newState.range = lhs.range - rhs.range;

  //Discrete Values
  newState.task = (lhs.task == rhs.task)?rhs.task:"";
  newState.block = (lhs.block == rhs.block)?rhs.block:BLOCK_AIR;
  newState.reachable = (lhs.reachable == rhs.reachable)?rhs.reachable:1-rhs.reachable;

  //Return the State
  return newState;
}

//Compare States, return a vector of keys?
State operator+(State lhs, const State& rhs) {
  State newState;
  //Continuous Values
  newState.pos = lhs.pos - rhs.pos;

  return newState;
}
