 //Include Header File
//Dependencies
#include "chunk.h"
#include "octree.h"
#include "../game/item.h"
#include "../render/shader.h"
#include "../render/billboard.h"
#include "../render/model.h"
#include "../render/view.h"
#include "world.h"

/*
===================================================
          WORLD GENERATING FUNCTIONS
===================================================
*/

void World::generate(){
  logger::write("Generating New World");

  //Set the Blueprint Properties
  blueprint.dim = dim;
  blueprint.chunkSize = chunkSize;

  //Generate the Blank Region Files
  blank();

  //Generate Height
  Terrain terrain(this);
  terrain.generate();
}

std::string World::regionString(glm::vec3 cpos){
  //Get the Region String from the Chunk's Position
  glm::vec3 rpos = glm::floor(cpos / region);
  stringstream ss;
  ss << rpos.x << rpos.y << rpos.z;
  return ss.str();
}

void World::blank(){
  //Loop over all Chunks in the World
  for(int i = 0; i < dim.x; i++){
    for(int j = 0; j < dim.y; j++){
      for(int k = 0; k < dim.z; k++){

        //Generate a new chunk at a specific location
        Chunk chunk;
        chunk.biome = BIOME_VOID;
        chunk.size = chunkSize;
        chunk.pos = glm::vec3(i, j, k);

        //Get the savefile Path
        boost::filesystem::path data_dir(boost::filesystem::current_path());
        data_dir /= "save";
        data_dir /= saveFile;
        data_dir /= "world.region"+regionString(chunk.pos);
        std::ofstream out(data_dir.string(), std::ofstream::app);

        if(format == SAVE_OCTREE)
        //Save the current loaded chunks to file and the world data
        {
          boost::archive::text_oarchive oa(out);

          Octree octree;
          octree.fromChunk(chunk);
          octree.trySimplify();
          octree.depth = 4;
          oa << chunk.pos;
          oa << octree;    //Append the Chunk to the Region File
        }
        else
        {
          boost::archive::text_oarchive oa(out);
          oa << chunk;    //Append the Chunk to the Region File
        }

        out.close();
      }
    }
  }
}

/*
===================================================
          BLUEPRINT HANDLING FUNCTIONS
===================================================
*/

bool World::evaluateBlueprint(Blueprint &_blueprint){
  //Check if the editBuffer isn't empty!
  if(_blueprint.editBuffer.empty()) return false;

  //Sort the editBuffer
  std::sort(_blueprint.editBuffer.begin(), _blueprint.editBuffer.end(), std::greater<bufferObject>());

  //Open the File
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "save";
  data_dir /= saveFile;
  data_dir /= "world.region";

  //Chunk for Saving Data
  Octree _octree;
  Chunk _chunk;
  int n_chunks = 0;

  // Loop over the EditBuffer
  while(!_blueprint.editBuffer.empty()){
    glm::vec3 a = _blueprint.editBuffer.back().cpos;

    //Load the Appropriate Locations
    std::ifstream in((data_dir.string()+regionString(a)));
    std::ofstream out((data_dir.string()+regionString(a)+".temp"), std::ofstream::app);

    int n = 0;
    while(n < region.x*region.y*region.z){

      //Load the Chunk
      if(format == SAVE_OCTREE){
        boost::archive::text_iarchive ia(in);
        ia >> _chunk.pos;
        ia >> _octree;
      }
      else
      {
        boost::archive::text_iarchive ia(in);
        ia >> _chunk;
      }

      bool edited = false;

      while(!_blueprint.editBuffer.empty() && glm::all(glm::equal(_chunk.pos, _blueprint.editBuffer.back().cpos))){
        if(format == SAVE_OCTREE) _octree.setPosition(glm::mod((glm::vec3)_blueprint.editBuffer.back().pos, glm::vec3(chunkSize)), _blueprint.editBuffer.back().type);
        else _chunk.setPosition(glm::mod((glm::vec3)_blueprint.editBuffer.back().pos, glm::vec3(chunkSize)), _blueprint.editBuffer.back().type);
        _blueprint.editBuffer.pop_back();
        edited = true;
      }

      //Write the chunk back
      if(format == SAVE_OCTREE){
        boost::archive::text_oarchive oa(out);

        //Don't forget to simplify this octree!!
        if(edited) _octree.trySimplify();  //This takes advantage of the sparsity.
        oa << _chunk.pos;
        oa << _octree;
      }
      else
      {
        boost::archive::text_oarchive oa(out);
        oa << _chunk;
      }

      n++;
    }

    //Close the fstream and ifstream
    in.close();
    out.close();

    //Delete the first file, rename the temp file
    boost::filesystem::remove_all((data_dir.string()+regionString(a)));
    boost::filesystem::rename((data_dir.string()+regionString(a)+".temp"), (data_dir.string()+regionString(a)));
  }

  return true;
}

/*
===================================================
          MOVEMENT RELATED FUNCTIONS
===================================================
*/

BlockType World::getBlock(glm::vec3 _pos){
  glm::vec3 c = glm::floor(_pos/glm::vec3(chunkSize));
  glm::vec3 p = glm::mod(_pos, glm::vec3(chunkSize));
  int ind = helper::getIndex(c, dim);

  //Get the Block with Error Handling
  if(!(glm::all(glm::greaterThanEqual(c, min)) && glm::all(glm::lessThanEqual(c, max)))) return BLOCK_VOID;
  return (BlockType)chunks[chunk_order[ind]].data[chunks[chunk_order[ind]].getIndex(p)];
}

void World::setBlock(glm::vec3 _pos, BlockType _type, bool fullremesh){
  glm::vec3 c = glm::floor(_pos/glm::vec3(chunkSize));
  glm::vec3 p = glm::mod(_pos, glm::vec3(chunkSize));
  int ind = helper::getIndex(c, dim);

  if(!(glm::all(glm::greaterThanEqual(c, min)) && glm::all(glm::lessThanEqual(c, max)))) return;
  chunks[chunk_order[ind]].setPosition(p, _type);

  //Add to the EditBuffer
  blueprint.addEditBuffer(_pos, _type, false);

  if(fullremesh) chunks[chunk_order[ind]].remesh = true;
  else remeshBuffer.addEditBuffer(_pos, _type, false);
}

//Get the Top-Free space position in the x-z position
glm::vec3 World::getTop(glm::vec2 _pos){
  //Highest Block you can Stand O
  int max = 0;
  BlockType floor;

  //Loop over the height
  for(int i = 1; i < dim.y*chunkSize; i++){
    floor = getBlock(glm::vec3(_pos.x, i-1, _pos.y));
    //Check if we satisfy the conditions
    if(getBlock(glm::vec3(_pos.x, i, _pos.y)) == BLOCK_AIR && block::isSpawnable(floor)){
      if(i > max){
        max = i;
      }
    }
  }
  return glm::vec3(_pos.x, max, _pos.y);
}

/*
===================================================
          DROPS AND PLACEMENT FUNCTIONS
===================================================
*/

//Drop all the items in the inventory
void World::drop(Inventory inventory){
  for(unsigned int i = 0; i < inventory.size(); i++){
    drops.push_back(inventory[i]);
  }
}

//Remove the items from the drops and
Inventory World::pickup(glm::vec3 pos){
  Inventory _inventory;
  for(unsigned int i = 0; i < drops.size(); i++){
    if(drops[i].pos == pos){
      //Add to inventory and remove from drops.
      _inventory.push_back(drops[i]);
      drops.erase(drops.begin()+i);
      i--;
    }
  }
  return _inventory;
}


/*
===================================================
          FILE IO HANDLING FUNCTIONS
===================================================
*/

void World::bufferChunks(View &view){
  lock = true;
  //Load / Reload all Visible Chunks
  evaluateBlueprint(blueprint);

  //Chunks that should be loaded
  min = glm::floor(view.viewPos/glm::vec3(chunkSize))-view.renderDistance;
  max = glm::floor(view.viewPos/glm::vec3(chunkSize))+view.renderDistance;

  //Can't exceed a certain size
  min = glm::clamp(min, glm::vec3(0), dim-glm::vec3(1));
  max = glm::clamp(max, glm::vec3(0), dim-glm::vec3(1));

  //Chunks that need to be removed
  std::stack<int> remove;
  std::vector<glm::ivec3> load;

  //Construct the Vector of guys we should load
  for(int i = min.x; i <= max.x; i ++){
    for(int j = min.y; j <= max.y; j ++){
      for(int k = min.z; k <= max.z; k ++){
        //Add the vector that we should be loading
        load.push_back(glm::ivec3(i, j, k));
      }
    }
  }

  //Loop over all existing chunks
  for(unsigned int i = 0; i < chunks.size(); i++){
    //Check if any of these chunks are outside of the limits of a / b
    if(glm::any(glm::lessThan(chunks[i].pos,  (glm::ivec3)min)) || glm::any(glm::greaterThan(chunks[i].pos,  (glm::ivec3)max))){
      //Add the chunk to the erase pile
      remove.push(i);
      continue;
    }

    //Make sure that the chunk that we determined will not be removed is also not reloaded
    for(unsigned int j = 0; j < load.size(); j++){
      if(glm::all(glm::equal(load[j], chunks[i].pos))){
        //Remove the element from load, as it is already inside this guy
        load.erase(load.begin()+j);
      }
    }
  }

  //Loop over the erase pile, delete the relevant chunks and models.
  while(!remove.empty()){
    chunks.erase(chunks.begin()+remove.top());
    view.models.erase(view.models.begin()+remove.top());
    remove.pop();
  }

  //Check if we want to load any guys
  if(!load.empty()){
    //Sort the loading vector, for single file-pass
    std::sort(load.begin(), load.end(),
            [](const glm::vec3& a, const glm::vec3& b) {
              if(a.x > b.x) return true;
              if(a.x < b.x) return false;

              if(a.y > b.y) return true;
              if(a.y < b.y) return false;

              if(a.z > b.z) return true;
              if(a.z < b.z) return false;

              return false;
            });

    //Open the Appropriate Region File...
    boost::filesystem::path data_dir(boost::filesystem::current_path());
    data_dir /= "save";
    data_dir /= saveFile;
    std::ifstream in;

    // We want to load certain chunk data
    while(!load.empty()){

      std::string curString = regionString(load.back());
      in.open((data_dir / ("world.region"+curString)).string());

      int n = 0;

      //Make sure we are in the same file
      while(curString == regionString(load.back())){

        //The variables need to reset here... **
        Octree _octree;
        Chunk _chunk;

        //Skip...
        while(n < helper::getIndex(glm::mod((glm::vec3)load.back(), region), region)){
          in.ignore(1000000,'\n');
          n++;
        }

        //Load the Chunk
        {
          boost::archive::text_iarchive ia(in);
          if(format == SAVE_OCTREE){
            ia >> _chunk.pos;
            ia >> _octree;
            _octree.trySimplify();
            _chunk.fromOctree(_octree, glm::vec3(0)); //** ..because of this!
            chunks.push_back(_chunk);
          }
          else{
            ia >> _chunk;
            chunks.push_back(_chunk);
          }
        }
        load.pop_back();
      }
      in.close();
    }
  }

  //Write the Chunk Order
  chunk_order.clear();
  for(int i = 0; i < chunks.size(); i++){
    //Set the Pointer to the chunk guy
    chunk_order[helper::getIndex(chunks[i].pos, dim)] = i;
  }

  //Update the Corresponding Chunk Models
  view.loadChunkModels(*this);
  lock = false;
}

bool World::loadWorld(){
  //Get current path
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "save";

  //Make sure savedirectory exists
  if(!boost::filesystem::is_directory(data_dir)){
    //Create the directory upon failure
    boost::filesystem::create_directory(data_dir);
  }

  //Check if the savefile directory exists
  data_dir /= saveFile;
  if(!boost::filesystem::is_directory(data_dir)){
    //Create the directory upon failure
    boost::filesystem::create_directory(data_dir);
    //Generate a new world!
    saveWorld();
    return true;
  }

  //Load the World Mete-
  data_dir /= "world.meta";
  std::ifstream in(data_dir.string());
  {
    boost::archive::text_iarchive ia(in);
    ia >> *this;
  }
  in.close();
  return true;
}

bool World::saveWorld(){
  //Get the location
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "save";
  data_dir /= saveFile;
  data_dir /= "world.meta";
  //Make new directory, save stuff to a world file
  std::ofstream out(data_dir.string());
  {
    boost::archive::text_oarchive oa(out);
    oa << *this;
  }
  generate();
  out.close();
  return true;
}


namespace boost {
namespace serialization {

//Vector Serializer
template<class Archive>
void serialize(Archive & ar, glm::ivec3 & _vec, const unsigned int version)
{
  ar & _vec.x;
  ar & _vec.y;
  ar & _vec.z;
}

//Chunk Serializer
template<class Archive>
void serialize(Archive & ar, Chunk & _chunk, const unsigned int version)
{
  ar & _chunk.pos;
  ar & _chunk.size;
  ar & _chunk.biome;
  ar & _chunk.data;
}

//Octree Serializer
template<class Archive>
void serialize(Archive & ar, Octree & _octree, const unsigned int version)
{
  ar & _octree.depth;
  ar & _octree.index;
  ar & _octree.type;
  ar & _octree.subTree;
}

//World Serializer
template<class Archive>
void serialize(Archive & ar, World & _world, const unsigned int version)
{
  ar & _world.saveFile;
  ar & _world.format;
  ar & _world.chunkSize;
  ar & _world.dim.x;
  ar & _world.dim.y;
  ar & _world.dim.z;
}

} // namespace serialization
} // namespace boost
