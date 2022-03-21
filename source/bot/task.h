/*
================================================================================
                            Task Bot System - Task
================================================================================

The idea is that I have a taskpool, which stores a number of available task objects.
These are the basic structures which determine something that is being done.

Tasks are abstract function pointers which do a number of things??
Or are they structs which contain the abstract function pointers??

Not sure.

How do I include metadata in the abstract class???

I guess the derived class can contain relevant information.

*/

#ifndef TERRITORY_TASK
#define TERRITORY_TASK

#include <functional>

class Bot;

using namespace std;
using namespace glm;

using TaskHandle = function<bool(Bot*)>;

/*
================================================================================
                               Task Base Class
================================================================================
*/

struct Task {

  Task(){};

  string desc = "";     //Task Description
  bool init = true;     //Initial Execution
  vector<Task*> tasks;  //Internal Task Queue

  bool queue(Bot*);     //Handle the Queue
  bool perform(Bot*);   //Multi-Step Task Performance
  TaskHandle handle;    //Actual Function Handle

  static World* world;

};

World* Task::world = NULL;

bool Task::perform(Bot* bot){

  if(handle(bot))       //If the Task Reaches its End Condition
    return true;        //The Task has been Performed

  init = false;         //No Longer first Performance
  return false;         //Task is NOT completed

};

bool Task::queue(Bot* bot){

  if(tasks.empty())               //Queue Handled
    return true;

  if(tasks.back()->perform(bot)){ //Perform Top Task
    delete tasks.back();
    tasks.pop_back();             //Remove from Queue
  }

  return false;                   //Repeat Loop

}

#endif
