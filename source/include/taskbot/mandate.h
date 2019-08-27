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
    Mandate(short _issuer, short _bot, short _group, const bool _repeat, short _urgency);

    //Mandate Meta Data
    State goal;           //Current Goal State
    short issuer;         //Bot UUID of Issuer Bot
    short bot;            //Bot UUID for "Responsible" Bot
    short group;          //Group UUID for "Responsible" group
    bool repeat;          //Whether to delete the mandate when goal state is true
    short urgency;        //Some changeable urgency value

    //Determined Values
    Task* suggest;        //Suggested Task
    short viability;      //Some value giving the suggested paths viability

    //Return a recommended task for a specific mandate.
    bool completed(World &world, Population &population, int bot);
    void getSuggest();                   //Get the suggested task list (in our case, fill with request)
    void getViability();                //Return the Viability of the suggested task
};
