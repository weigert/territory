#pragma once
#include "../forward/blueprint.fwd.h"

//Types of Buildings
enum BlueprintType{
  BP_HOUSE,
  BP_BRIDGE,
  BP_TREE,
  BP_BUSH
  //etc...
};

enum BuildingStyle{
  RUSTIC,
  CASTLE
};

//EditBuffer Struct
struct bufferObject {
  glm::ivec3 pos;
  glm::ivec3 cpos;
  glm::ivec2 rpos;
  BlockType type;
};

//Sorting Operator for bufferObjects
bool operator>(const bufferObject& a, const bufferObject& b) {
  //Sort by Region Position
  if(a.rpos.x > b.rpos.x) return true;
  if(a.rpos.x < b.rpos.x) return false;

  if(a.rpos.y > b.rpos.y) return true;
  if(a.rpos.y < b.rpos.y) return false;

  //Sort by Chunk Position
  if(a.cpos.x > b.cpos.x) return true;
  if(a.cpos.x < b.cpos.x) return false;

  if(a.cpos.y > b.cpos.y) return true;
  if(a.cpos.y < b.cpos.y) return false;

  if(a.cpos.z > b.cpos.z) return true;
  if(a.cpos.z < b.cpos.z) return false;

  return false;
}

class Blueprint{
public:
  //Dimension Parameters
  static int chunkSize;
  static glm::vec3 dim;

  //Helpers
  std::vector<bufferObject> editBuffer;
  bool addEditBuffer(glm::vec3 _pos, BlockType _type, bool negative);
  void removeDuplicates(bool later);
  void merge(Blueprint _other);
  Blueprint translate(glm::ivec3 _pos);

  //Savers and Loaders
  void save(std::string savefile);
  void load(std::string loadfile);

  //Blueprint Constructors
  void flatSurface(int x, int z);
  void hut();

  template <BuildingStyle S>
  void building(const int &n);

  template<BuildingStyle S>
  bool buildingFromGraph(graph::Graph<graph::ROOMGRAPH> buildingGraph);

  void tree(int height);
  void cactus();
  void boulder();
};

//Default Initialization
int Blueprint::chunkSize = 16;
glm::vec3 Blueprint::dim = glm::vec3(20,5,20);

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, bufferObject &_buf, const unsigned int version)
{
  ar & _buf.pos.x;
  ar & _buf.pos.y;
  ar & _buf.pos.z;
  ar & _buf.cpos.x;
  ar & _buf.cpos.y;
  ar & _buf.cpos.z;
  ar & _buf.type;
}

template<class Archive>
void serialize(Archive & ar, Blueprint &_blueprint, const unsigned int version)
{
  ar & _blueprint.chunkSize;
  ar & _blueprint.dim.x;
  ar & _blueprint.dim.y;
  ar & _blueprint.dim.z;
  ar & _blueprint.editBuffer;
}

} // namespace serialization
} // namespace boost
