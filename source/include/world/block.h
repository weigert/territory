/*
We need Block-Metadata:

-> Blocks should be able to have different properties, including:

=> Orientation

A few options:
-> Store these as boolean flags or similar in some block struct,
or: Because every block of a specific type will have the same properties (at least for a few),
we should be able to retrieve it using a function.
*/

enum BlockType{
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

  glm::vec4 getColor(BlockType _type, double t){
    //Switch the value and return a vector
    switch(_type){
      case BLOCK_GRASS:
        return color::bezier(t, color::grasscolors);
        break;
      case BLOCK_DIRT:
        return glm::vec4(0.74f, 0.5f, 0.36f, 1.0f);
        break;
      case BLOCK_WATER:
        return glm::vec4(0.3f, 0.57f, 0.67f, 0.8f); //Semi Transparent!
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
        return glm::vec4(0.6f, 0.375f, 0.14f, 1.0f);
        break;
      case BLOCK_GRAVEL:
      return color::bezier(t, color::gravelcolors);
        break;
      case BLOCK_SANDSTONE:
        return glm::vec4(0.8f, 0.75f, 0.64f, 1.0f);
        break;
      case BLOCK_PUMPKIN:
        return glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
        break;
      case BLOCK_CACTUS:
        return glm::vec4(0.0f, 0.44f, 0.3f, 1.0f);
        break;
      case BLOCK_PLANKS:
        return glm::vec4(0.75f, 0.6f, 0.28f, 1.0f);
        break;
      case BLOCK_GLASS:
        return glm::vec4(0.8f, 0.9f, 0.95f, 0.2f);
        break;
      default:
        return glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
        break;
    }
  }
};
