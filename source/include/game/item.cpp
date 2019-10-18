#include "../taskbot/task.h"
#include "../render/sprite.h"
#include "../taskbot/memory.h"
#include "../taskbot/state.h"
#include "../world/world.h"
#include "../taskbot/population.h"
#include "../taskbot/bot.h"

#include "item.h"

void Item::setupSprite(){
  //This needs to be parameterized by the item itself...
  sprite.loadImage("item.png");
  sprite.setupBuffer(true);
  sprite.resetModel();
}

void Item::fromTable(BlockType _block){
  if(_block == BLOCK_WOOD){
    name = "Wooden Logs";
    _type = LOGS;
    quantity = 1;
  }
  if(_block == BLOCK_STONE){
    name = "Stone Rocks";
    _type = ROCKS;
    quantity = 1;
  }
}
