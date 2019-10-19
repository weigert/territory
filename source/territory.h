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

enum BiomeType{
  BIOME_VOID = 0,
  BIOME_DESERT = 1,
  BIOME_FOREST = 2,
};

enum BlockType{
  BLOCK_AIR = 0,
  BLOCK_GRASS = 1,
  BLOCK_DIRT = 2,
  BLOCK_SAND = 3,
  BLOCK_CLAY = 4,
  BLOCK_GRAVEL = 5,
  BLOCK_STONE = 6,
  BLOCK_WATER = 7,
  BLOCK_LEAVES = 8,
  BLOCK_WOOD = 9,
  BLOCK_PUMPKIN = 10,
};

//Dear IMgui Dependencies
#include "include/imgui/imgui.h"
#include "include/imgui/imgui_impl_sdl.h"
#include "include/imgui/imgui_impl_opengl3.h"

//Drawing Dependencies
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

//Containers, Math, Other
#include <stdlib.h>
#include <string>
#include <noise/noise.h>
#include <vector>
#include <deque>

//File / General IO
#include <iostream>
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

//Helper Functions
#include "include/helpers/arraymath.h"
#include "include/helpers/CImg.h"

//Logger
#include "include/game/logger.cpp"
Logger _log;

//Pathfinding Classes
#include "include/astar/sltastar.h"
#include "include/astar/astar.h"

//World Classes
#include "include/world/octree.cpp"
#include "include/world/chunk.cpp"
#include "include/world/world.cpp"

using namespace cimg_library;
#include "include/world/geology.cpp"

//View Classes
#include "include/render/interface.cpp"
#include "include/render/billboard.cpp"
#include "include/render/view.cpp"
#include "include/render/model.cpp"
#include "include/render/sprite.cpp"
#include "include/render/shader.cpp"

//Game Classes
#include "include/game/event.cpp"
#include "include/game/item.cpp"
#include "include/game/player.cpp"

//NPC Classes
#include "include/taskbot/state.cpp"
#include "include/taskbot/memory.cpp"
#include "include/taskbot/bot.cpp"
#include "include/taskbot/mandate.cpp"
#include "include/taskbot/population.cpp"
#include "include/taskbot/task.cpp"
