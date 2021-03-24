#ifndef TERRITORY_BLOCK
#define TERRITORY_BLOCK

enum BlockType: unsigned char {
  BLOCK_AIR,
  BLOCK_GRASS,
  BLOCK_DIRT,
  BLOCK_SAND,
  BLOCK_CLAY,
  BLOCK_GRAVEL,
  BLOCK_SANDSTONE,
  BLOCK_STONE,
  BLOCK_WATER,
  BLOCK_LEAVES,
  BLOCK_WOOD,
  BLOCK_PUMPKIN,
  BLOCK_CACTUS,
  BLOCK_PLANKS,
  BLOCK_GLASS,
  BLOCK_VOID,
  BLOCK_CACTUSFLOWER
};

namespace color {
using namespace std;
using namespace glm;

  vector<vec4> skycolors{
    vec4(0.1, 0.1, 0.1, 1.0),            //Dark Grey
    vec4(0.00, 1.00, 1.00, 1.0),         //Blue
    vec4(1.00, 1.00, 1.00, 1.0),         //White
    vec4(0.1, 0.1, 0.1, 1.0)             //Dark Grey
  };

  vector<vec4> sandcolors{
    vec4(1.00, 0.75, 0.35, 1.0),
    vec4(0.84, 0.21, 0.21, 1.0),
    vec4(0.93, 0.91, 0.38, 1.0)
  };

  vector<vec4> grasscolors{
    vec4(0.65, 0.72, 0.34, 1.0),
    vec4(0.19, 0.45, 0.34, 1.0),
    vec4(0.54, 0.70, 0.34, 1.0)
  };

  vector<vec4> stonecolors{
    vec4(0.5, 0.5, 0.5, 1.0),
    vec4(0.6, 0.6, 0.6, 1.0),
    vec4(0.7, 0.7, 0.7, 1.0)
  };

  vector<vec4> gravelcolors{
    vec4(0.9, 0.9, 0.9, 1.0),
    vec4(0.8, 0.8, 0.8, 1.0),
    vec4(0.7, 0.7, 0.7, 1.0)
  };

  vector<vec4> autumncolors{
    vec4(0.17, 0.40, 0.26, 0.8),
    vec4(1.00, 0.75, 0.35, 1.0),
    vec4(0.84, 0.21, 0.21, 1.0)
  };

  vector<vec4> leafcolors{
    vec4(0.16, 0.44, 0.27, 1.0),
    vec4(0.05, 0.31, 0.22, 1.0),
    vec4(0.35, 0.43, 0.13, 1.0)
  };

  vector<vec4> claycolors{
    vec4(0.97f, 0.5f, 0.44f, 1.0f),
    vec4(0.75, 0.30, 0.30, 1.0),
  };

}


//Block Property Handling Namespace
namespace block{
  //Setting Getters
  bool isVisible(BlockType type){
    return (type != BLOCK_AIR && type != BLOCK_VOID);
  }

  //Is the Blocks Model a Cube?
  bool isCubic(BlockType type){
    return type != BLOCK_CACTUSFLOWER;
  }

  //Will this block information be written to a bot's memory?
  bool isMemorable(BlockType type){
    return
      type != BLOCK_AIR &&
      type != BLOCK_STONE &&
      type != BLOCK_DIRT &&
      type != BLOCK_GRASS &&
      type != BLOCK_SAND;
  }

  //Can a bot be spawned on this block?
  bool isSpawnable(BlockType type){
    return (type == BLOCK_GRASS || type == BLOCK_SAND);
  }

  bool isDestructable(BlockType type){
    return type != BLOCK_AIR;
  }

  bool isFellable(BlockType type){
    return type == BLOCK_WOOD || type == BLOCK_CACTUS;
  }

  //Can the bot be "inside" this block?
  bool isOccupiable(BlockType type){
    return type == BLOCK_AIR;
  }

  //Block Moveweights
  int moveWeight(BlockType type){
    switch(type){
      case BLOCK_AIR:
        return 1;
      default:
        return 10;
    }
  }

  vec4 getColor(BlockType _type, double t){
    //Switch the value and return a vector
    switch(_type){
      case BLOCK_GRASS:
        return color::bezier(t, color::grasscolors);
        break;
      case BLOCK_DIRT:
        return vec4(0.74f, 0.5f, 0.36f, 1.0f);
        break;
      case BLOCK_WATER:
        return vec4(0.3f, 0.57f, 0.67f, 0.8f); //Semi Transparent!
        break;
      case BLOCK_SAND:
        return color::bezier(t, color::sandcolors);
        break;
      case BLOCK_CLAY:
        return color::bezier(t, color::claycolors);
        break;
      case BLOCK_STONE:
        return color::bezier(t, color::stonecolors);
        break;
      case BLOCK_LEAVES:
        return color::bezier(t, color::leafcolors);
        break;
      case BLOCK_WOOD:
        return vec4(0.6f, 0.375f, 0.14f, 1.0f);
        break;
      case BLOCK_GRAVEL:
      return color::bezier(t, color::gravelcolors);
        break;
      case BLOCK_SANDSTONE:
        return vec4(0.8f, 0.75f, 0.64f, 1.0f);
        break;
      case BLOCK_PUMPKIN:
        return vec4(1.0f, 0.5f, 0.0f, 1.0f);
        break;
      case BLOCK_CACTUS:
        return vec4(0.0f, 0.44f, 0.3f, 1.0f);
        break;
      case BLOCK_PLANKS:
        return vec4(0.75f, 0.6f, 0.28f, 1.0f);
        break;
      case BLOCK_GLASS:
        return vec4(0.8f, 0.9f, 0.95f, 0.2f);
        break;
      default:
        return vec4(1.0f, 1.0f, 1.0f, 0.5f);
        break;
    }
  }
};

#endif
