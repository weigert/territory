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
  BLOCK_WOOD = 9
};

class Octree{
public:
  Octree(){
    type = BLOCK_AIR;
    index = 0;
  }

  //Some Information contained by the octree element
  std::vector<Octree> subTree;
  int depth;    //Gives us the remaining depth levels of the octree
  int index;

  //By default type is 0, and subTree is empty.
  BlockType type;

  //Filling and Combining Operations for Subtrees
  bool trySimplify();
  bool contains(glm::vec3 _pos);

  //Index Functions
  glm::vec3 getPos(int index);
  int getIndex(glm::vec3 _pos);

  //Get Color Data
  glm::vec4 getColorByID(BlockType _type);

  //We need setters, getters, volume setters, etc.
  bool setPosition(glm::vec3 _pos, BlockType _type);
  BlockType getPosition(glm::vec3 _pos, int LOD);
};
