#include "../taskbot/task.h"
#include "../render/sprite.h"
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

bool Item::fromTable(BlockType _block){
  if(_block == BLOCK_WOOD){
    name = "Wooden Logs";
    _type = LOGS;
    quantity = 1;
    return true;
  }
  if(_block == BLOCK_STONE){
    name = "Stone Rocks";
    _type = ROCKS;
    quantity = 1;
    return true;
  }
  if(_block == BLOCK_PUMPKIN){
    name = "Pumpkin";
    _type = PUMPKIN;
    quantity = 1;
    return true;
  }
  if(_block == BLOCK_LEAVES){
    name = "Leaves";
    _type = PUMPKIN;
    quantity = 1;
    return true;
  }
  if(_block == BLOCK_CLAY){
    name = "Clay";
    _type = ROCKS;
    quantity = 1;
    return true;
  }
  return false;
}

//Merge the Inventories
void mergeInventory(Inventory &_into, Inventory _merge){
  //Loop over the stuff that we picked up, and sort it into the inventory
  for(unsigned int i = 0; i < _merge.size(); i++){
    bool found = false;
    for(unsigned int j = 0; j < _into.size(); j++){
      if(_merge[i]._type == _into[j]._type){
        _into[j].quantity += _merge[i].quantity;
        found = true;
      }
    }
    //We haven't found the item in the inventory, so add it to the back
    if(!found){
      _into.push_back(_merge[i]);
    }
  }
}
