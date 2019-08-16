//Pathfinding Stuff
#pragma once
#include "../forward/world.fwd.h"
#include "../forward/population.fwd.h"
#include "../forward/bot.fwd.h"
#include "../forward/state.fwd.h"

using namespace std;

std::vector<glm::vec3> calculatePath(int id, glm::vec3 _dest, Population &population, World &world);
using namespace std::placeholders;

/*
New Tasks:
say (communicate a thought to someone else)
listen (write short term memory to long-term memory)
think (for a number of ticks, query memory repeatedly to increase recallScore)
  think can also help sort and prioritize memories?
travel - multiple step journey (long distances).
follow (walk to individual)
stalk (= follow at distance)
sleep (= wait + dream)
dream (= think)
hit (adjacent being)
attack (= continuous follow and hit)
hunt (= continuous stalk and hit)
*/

//Task Class
class Task{
  public:
    //Members
    std::stack<Task> queue;
    bool initFlag = true;
    int args[10];

    bool (Task::*handle)(World&, Population&, int (&)[10]);
    int botID;
    std::string name;

    //Constructor
    Task(std::string taskName, int taskBotID, bool (Task::*taskHandle)(World&, Population&, int (&)[10]));

    //Launch a Task
    bool perform(World &world, Population &population);

    //Primitives
    bool wait(World &world, Population &population, int (&arguments)[10]);
    bool look(World &world, Population &population, int (&arguments)[10]);
    bool step(World &world, Population &population, int (&arguments)[10]);
    bool swap(World &world, Population &population, int (&arguments)[10]);
    bool store(World &world, Population &population, int (&arguments)[10]);
    bool consume(World &world, Population &population, int (&arguments)[10]);
    bool move(World &world, Population &population, int (&arguments)[10]);

    //Secondaries
    bool walk(World &world, Population &population, int (&arguments)[10]);
    bool idle(World &world, Population &population, int (&arguments)[10]);
    bool search(World &world, Population &population, int (&arguments)[10]);
    bool forage(World &world, Population &population, int (&arguments)[10]);
    bool take(World &world, Population &population, int (&arguments)[10]);

    //Example Task
    bool Dummy(World &world, Population &population, int (&arguments)[10]);
};
