//Pathfinding Stuff
#pragma once
#include "../forward/world.fwd.h"
#include "../forward/population.fwd.h"
#include "../forward/bot.fwd.h"
#include "../forward/state.fwd.h"

using namespace std;

std::vector<glm::vec3> calculatePath(int id, glm::vec3 _dest, Population &population, World &world);
using namespace std::placeholders;

//Task Class
class Task{
  public:
    //Members
    std::stack<Task> queue;
    bool initFlag = true;
    State args;

    bool (Task::*handle)(World&, Population&, State&);
    int botID;
    std::string name;
    int animation = 0;
    glm::vec3 translate = glm::vec3(0);

    //Constructor
    Task(std::string taskName, int taskBotID, bool (Task::*taskHandle)(World&, Population&, State&));
    Task(std::string taskName, int taskBotID, int animationID, glm::vec3 animationTranslate, bool (Task::*taskHandle)(World&, Population&, State &_args));

    //Launch a Task
    bool perform(World &world, Population &population);
    bool handleQueue(World &world, Population &population);

    //0-Level Algorithmic
    bool wait(World &world, Population &population, State &_args);
    bool look(World &world, Population &population, State &_args);
    bool step(World &world, Population &population, State &_args);
    bool move(World &world, Population &population, State &_args);

    //Error-Handling Primaries
    bool walk(World &world, Population &population, State &_args);
    bool idle(World &world, Population &population, State &_args);
    bool search(World &world, Population &population, State &_args);
    bool forage(World &world, Population &population, State &_args);

    //Behaviors
    bool Dummy(World &world, Population &population, State &_args);
};
