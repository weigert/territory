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

enum ItemType{
  LOGS, ROCKS, PUMPKIN, CLAY, WATER, SAND, DIRT, GRAVEL, SALT,
  VINES, REEDS, GRASS, HERBS, FLOWERS, SPICES,
  CARROT, CABBAGE, POTATO, COTTON, APPLE,
  STRAWBERRY, GRAPE, BLUEBERRY,
  CIDER, WINE, BEER,
  GLASS, BRICK, FABRIC, POT, ROPE, PLANKS,
  BARREL, CHEST, BED, CARPET, CHAIR, TABLE, STOVE, LANTERN,
  PAPER, QUILL, BOOK, CONTRACT, PLAN, LEDGER, MAP,
  INSTRUMENT, GRAMMOPHONE, SUNDIAL, WATCH, OCTANT, THERMOMETER, COMPASS,
  WALLET, LOCKPICK, WATERSKIN, LENS, LOCKBOX,
  CLOCK, CANDLE, MICROSCOPE,
  WHEELBARROW, PLOT, SLED, WAGON, WHEEL,
  POTTERWHEEL, GLASSFORGE, METALFORGE, CRAFTERTABLE, BREWTABLE, LOOM, SPINDLE,
  CARPENTRYBENCH, TANNERRACK, PLANNINGTABLE, BUTCHERSLAB, ANVIL,                                          //Maps and architecture and shipmaking
  ORE, INGOT, GEM, COIN,
  PELT, MEAT, BONE, MILK, FEATHERS, FAT, HAIR, TOOTH,
  PANTS, SHIRT, COAT, HOOD, HAT, CAPE, GLOVES, BOOTS,
  AXE, PICKAXE, HOE, SHOVEL, BUCKET, CHISEL, SPIKE,
  SABER, KNIFE, PISTOL, RIFLE
};

enum ItemClass{
  NATURAL, FARMING, ANIMAL, PROCESSED, FURNITURE, WEAPON, CLOTHING, WORKSHOP
};

enum ItemQuality{
  LEGENDARY, PERFECT, GOOD, NORMAL, POOR, TERRIBLE, BROKEN
};

enum ItemStatus{
  WORN, PLACED, DROPPED, HELD, CONTAINED
};

enum ItemAbility{
  CONTAINER, INTERACTIVE, READABLE, PLACEABLE, EDIBLE, PLANTABLE, FORGEABLE,
  COST, REWARD, REQUIREMENT
};

enum ItemMaterial{
  WOOD, STONE, LEATHER       //Something?
};

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
