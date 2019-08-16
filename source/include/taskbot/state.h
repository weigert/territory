#pragma once
#include "../forward/state.fwd.h"
#include "../forward/world.fwd.h"
#include "../forward/population.fwd.h"
#include "../forward/bot.fwd.h"

class State{
  public:
    //Constructor
    State();
    State(glm::vec3 _loc, std::string _task, int _carry);

    void retrieveState(World &world, Population &population, int bot);

    //Basic set of mandate requirements
    glm::vec3 pos;
    std::string task;
    int carry;

    //This should be expanded for bot status conditions,
    //So they can learn about status conditions and their actions effects on them.

    //Compare States, return a vector of keys?
    bool operator==(const State& state) {
      if(pos != state.pos){
        return false;
      }
      if(task != state.task){
        return false;
      }
      if(carry != state.carry){
        return false;
      }
      return true;
    }

    //Compare States, return a vector of keys?
    State operator-(const State& state) {
      State newState;
      //Continuous Values
      newState.pos = pos - state.pos;

      //Enumerated Values
      (task != state.task)?newState.task=state.task:newState.task="";
      (carry != state.carry)?newState.carry=state.carry:newState.carry=0;

      return newState;
    }
};
