//Mandate Handling Class//Memory Handling Class
#pragma once
#include "../forward/task.fwd.h"
#include "../forward/state.fwd.h"
#include "../forward/world.fwd.h"
#include "../forward/population.fwd.h"
#include "../forward/mandate.fwd.h"

class Mandate{
  public:

    //Constructor
    Mandate(short _issuer, short _bot, short _group, const bool _repeat, Task *_request, short _urgency);

    //Mandate Meta Data
    short issuer;         //Bot UUID of Issuer Bot
    short bot;            //Bot UUID for "Responsible" Bot
    short group;          //Group UUID for "Responsible" group
    bool repeat;          //Whether to delete the mandate when goal state is true

    //Some information about goal state
    Task* request;        //Requested Task
    short urgency;        //Some changeable urgency value

    //Some information about suggested state.
    Task* suggest;        //Suggested Task
    short viability;      //Some value giving the suggested paths viability

    //Some Information About the Global State
    State goal;           //Current Goal State
    State dCurr;          //Achieved Difference to Current State
    State curr;           //Current State upon Acceptance
    State dGoal;          //Achieved Difference to Goal State

    void initStates(World &world, Population &population, int bot);
    void setGoal(World &world, Population &population, int bot);

    //Return a recommended task for a specific mandate.
    bool completed(World &world, Population &population, int bot);
    void getSuggest();                   //Get the suggested task list (in our case, fill with request)
    short getViability();                //Return the Viability of the suggested task
};
