//Memory Handling Class
#pragma once
#include "../forward/memory.fwd.h"
#include "../forward/state.fwd.h"

class Memory{
  public:
    //RecallScore for the Memory
    int recallScore = 1;

    //Storage of the State
    State state;

    //All Elements of Query Contained in Memory
    bool operator==(const Memory& query) {
      if(!(query.state == state)){
        return false;
      }
      return true;
    }

    //Any Elements of Query Contained in Memory
    bool operator||(const Memory& query) {
      if(!(query.state || state)){
        return false;
      }
      return true;
    }

    //Overwrite Memory Operator (only overwrites relevant portions)
    void operator=(const Memory& memory) {
      //Set the States
      state = memory.state;
    }
};
