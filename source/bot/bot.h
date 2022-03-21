/*
================================================================================
                            Task Bot System - Bot
================================================================================
*/

#ifndef TERRITORY_BOT
#define TERRITORY_BOT

using namespace std;
using namespace glm;

class Bot {
public:

Bot(vec3 _pos){
  pos = _pos;
}

//Base Properties
bool alive = true;
vec3 pos = vec3(0);

//Task Properties / Constants
const vec3 range = vec3(1,2,1);
const vec3 viewdist = vec3(2);

//Tasking System
Task* task = NULL;
bool execute();
bool interrupted = false;
bool interrupt();

};

bool Bot::interrupt(){
  interrupted = true;     //Interrupt always succeeds
  return interrupted;
}

bool Bot::execute(){

  if(!alive) return false;      //Dead Bots don't do Tasks

  //Check if bot is in relevant bounds!

  if(task == NULL)
    return true;

  if(interrupted){
    interrupted = false;
    return true;
  }

  else if(task->perform(this)){
    std::cout<<"Completed"<<std::endl;
    delete task;
    task = NULL;
    return true;
  }

  return false;

}

#endif
