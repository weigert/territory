//Forward Declarations
#include "../render/sprite.h"

#pragma once
#include "../forward/sprite.fwd.h"
#include "../forward/item.fwd.h"
#include "../forward/task.fwd.h"
#include "../forward/world.fwd.h"
#include "../forward/population.fwd.h"
#include "../forward/bot.fwd.h"
#include "../forward/state.fwd.h"
#include "../forward/memory.fwd.h"

typedef std::vector<Item> Inventory;

class Item{
  public:
    //Item Information
    std::string name;
    ItemType _type;
    int variant;
    int quantity;
    glm::vec3 pos;

    //Flags
    ItemClass _itemClass;
    ItemQuality _itemQuality;
    ItemStatus _itemStatus;
    std::vector<ItemAbility> _itemAbility;    //Multiple abilities
    std::vector<ItemMaterial> _itemMaterial;  //and materials!

    //Tasks, Memories, Items Container
    Task* interaction;
    std::vector<State> states;  //for the interaction!
    std::vector<Memory> inscription;
    Inventory inventory;

    //Sprite
    Sprite sprite;
    void setupSprite();

    //Members
    //randomize this item
    //needs to be generated from a specific block type.
    bool fromTable(BlockType _block);
};

//Comparison Operators
bool operator==(const Item& rhs, const Item&lhs) {
  //Set the States
  return rhs._type == lhs._type;
}
