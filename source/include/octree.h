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
    isNode = true;
    isRoot = false;
    type = BLOCK_AIR;
  }

  Octree(int _maxDepth){
    maxDepth = _maxDepth;
    isNode = true;
    isRoot = false;
    type = BLOCK_AIR;
  }

  //Some Information contained by the octree element
  std::vector<Octree> subTree;
  int maxDepth; //Our Chunk is at maximum 16^3 = 2^4^3
  bool isRoot;  //No parent
  bool isNode;  //No children (important for file IO)
  int depth;  //Gives us the depth inside the max octree

  //By default type is 0, and subTree is empty.
  BlockType type;

  void setRoot();
  bool contains(int x, int y, int z);

  //Filling and Combining Operations for Subtrees
  void fillSubTree();
  bool trySimplify();
  bool tryMajority(); //This function should search the octree and find majority values, so that we can do a lower LEVEL-OF-DETAL (LOD) map rendering later on.
  //Alternatively,  make trySimplify do that on its own.

  //We need setters, getters, volume setters, etc.
  bool setPosition(int x, int y, int z, BlockType _type);
  BlockType getPosition(int x, int y, int z);
};
