#pragma once
#include "../forward/state.fwd.h"
#include "../forward/world.fwd.h"
#include "../forward/population.fwd.h"
#include "../forward/bot.fwd.h"

class State{
  public:
    //Constructor
    State();

    //Basic set of mandate requirements
    glm::vec3 pos;
    std::string task;
    int carry;
    //int owner; //Owner of this Memory
    //int starter; //Originator of this Memory
    BlockType block;
    bool reachable;
    //Item item;
    int time;
    int dist;

    //Get the Current Local State
    void retrieveState(World &world, Population &population, int bot);

    //Overloaded Operators
    void operator=(const State& rhs) {
      //Set all properties
      pos = rhs.pos;
      task = rhs.task;
      carry = rhs.carry;
      block = rhs.block;
      reachable = rhs.reachable;
      time = rhs.time;
      dist = rhs.dist;
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
  if(lhs.carry != rhs.carry){
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
  if(lhs.dist != rhs.dist){
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
  if(lhs.carry == rhs.carry){
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
  if(lhs.dist == rhs.dist){
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
  newState.dist = lhs.dist - rhs.dist;

  //Discrete Values
  newState.carry = (lhs.carry == rhs.carry)?rhs.carry:0;
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
