/*
================================================================================
                            Task Bot System - Task
================================================================================
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
  vector<Task> tasks;   //Internal Task Queue

  bool queue(Bot*);     //Handle the Queue
  bool perform(Bot*);   //Multi-Step Task Performance
  TaskHandle handle;    //Actual Function Handle

};

bool Task::perform(Bot* bot){

  if(handle(bot))       //If the Task Reaches its End Condition
    return true;        //The Task has been Performed

  init = false;         //No Longer first Performance
  return false;         //Task is NOT completed

};

bool Task::queue(Bot* bot){

  if(tasks.empty())             //Queue Handled
    return true;

  if(tasks.back().perform(bot)) //Perform Top Task
    tasks.pop_back();           //Remove from Queue

  return false;                 //Repeat Loop

}

/*
================================================================================
                            Derived / Composed Tasks
================================================================================
*/

namespace task {

//List of All Available Tasks

struct Idle;
struct Example;

/*
================================================================================
                              Define the Tasks
================================================================================
*/

struct Idle : public Task {

  string desc = "Idle";

  Idle(){

    handle = [&](Bot* bot){
      cout<<"Handling Queue in Idle"<<endl;
      return true;        //Work off Queue
    };

  };

};

struct Example : public Task {

  string desc = "Example";

  Example(){

    tasks.push_back(Idle());    //Construct List of Tasks
    tasks.push_back(Idle());
    tasks.push_back(Idle());

    handle = [&](Bot* bot){
      cout<<"Handling Queue in Example"<<endl;
      return queue(bot);        //Work off Queue
    };

  };

};

} //End of Namespace


#endif
