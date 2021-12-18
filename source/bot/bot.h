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
void execute();
bool interrupted = false;
bool interrupt();

};

bool Bot::interrupt(){
  interrupted = true;     //Interrupt always succeeds
  return interrupted;
}

void Bot::execute(){

  if(!alive) return;      //Dead Bots don't do Tasks

  //Check if bot is in relevant bounds!

  if(task == NULL){
    task = new task::Example();
    return;
  }

  if(interrupted){
    interrupted = false;
    //set currrent task to decide
  }

  else if(task->perform(this)){
    std::cout<<"Completed"<<std::endl;
    delete task;
    task = new task::Example();
  }

}

#endif
