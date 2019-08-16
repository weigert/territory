//Memory Handling Class
#pragma once
#include "../forward/memory.fwd.h"

class Memory{
  public:
    int recallScore = 1;
    //Memory Query Array
    bool queryable[4] = {false, false, false, false}; //Has the length of all properties, to check if there is a value stored.
    //Memory Attributes
    BlockType object;
    std::string task;
    glm::vec3 location;
    bool reachable;

    //All Elements of Query Contained in Memory
    bool operator==(const Memory& query) {
      //Check All Elements of Memory (this is inefficient)
      if(query.queryable[0]){
        if(!(query.object == object)){
          return false;
        }
      }
      if(query.queryable[1]){
        if(query.task != task){
          return false;
        }
      }
      if(query.queryable[2]){
        if(query.location != location){
          return false;
        }
      }
      if(query.queryable[3]){
        if(query.reachable != reachable){
          return false;
        }
      }
      return true;
    }

    //Any Elements of Query Contained in Memory
    bool operator||(const Memory& query) {
      //Check All Elements of Memory (this is inefficient)
      if(query.queryable[0]){
        if(query.object == object){
          return true;
        }
      }
      if(query.queryable[1]){
        if(query.task == task){
          return true;
        }
      }
      if(query.queryable[2]){
        if(query.location == location){
          return true;
        }
      }
      if(query.queryable[3]){
        if(query.reachable == reachable){
          return true;
        }
      }
      return false;
    }

    //Overwrite Memory Operator (only overwrites relevant portions)
    void operator=(const Memory& memory) {
      if(memory.queryable[0]){
        object = memory.object;
      }
      if(memory.queryable[1]){
        task = memory.task;
      }
      if(memory.queryable[2]){
        location = memory.location;
      }
      if(memory.queryable[3]){
        reachable = memory.reachable;
      }
    }
};
