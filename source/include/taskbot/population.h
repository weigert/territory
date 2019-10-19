//Include Forward Declarations of Bot
#pragma once
#include "../forward/population.fwd.h"
#include "../forward/bot.fwd.h"
#include "../forward/world.fwd.h"

//Population Class
class Population{
  public:
    //Functions
    Population(World &world);
    void update(World &world, View view);

    //Population Functions
    void addBot(World world);
    void removeBot(int id);

    //Bot Container
    std::deque<Bot> bots;
};
