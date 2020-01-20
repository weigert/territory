#pragma once
#include "../forward/interface.fwd.h"
#include "../forward/population.fwd.h"
#include "../forward/world.fwd.h"
#include "../forward/player.fwd.h"
#include "../forward/sprite.fwd.h"
#include "../forward/bot.fwd.h"
#include "../forward/state.fwd.h"
#include "../forward/task.fwd.h"
#include "../forward/view.fwd.h"
#include "../forward/item.fwd.h"

class Interface{
public:
  int curID;
  //Render the Interface Elements
  void render(View &view, World &world, Population &population);

  //Extra Stuff
  void drawBot(Bot &bot);
  void drawTask(Task *task);
  void drawState(State state);
  void drawItem(Item item);

  //I also need creator interfaces for all these guys!!
  void createItem(Item &item);
  void createState(State &state);
  void createTask(Task &task);
};
