//Include Header File
//Dependencies
#include "chunk.h"
#include "octree.h"
#include "../game/item.h"
#include "../render/shader.h"
#include "../render/billboard.h"
#include "../render/view.h"
#include "world.h"

/*
===================================================
          WORLD GENERATING FUNCTIONS
===================================================
*/

void World::generate(){
  std::cout<<"Generating New World"<<std::endl;
  std::cout<<"Seed: "<<SEED<<std::endl;
  //Seed the Random Generator
  srand(SEED);

  //Generate the Blank Region Files
  std::cout<<"Generating Blank World"<<std::endl;
  generateBlank();

  //Generate Height
  std::cout<<"Filling World"<<std::endl;
  generateBuildings();
  //generatePerlin();
}

void World::generateBlank(){
  //Get the savefile Path
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "save";
  data_dir /= saveFile;
  data_dir /= "world.region";
  std::ofstream out(data_dir.string(), std::ofstream::app);

  //Loop over all Chunks in the World
  for(int i = 0; i < dim.x; i++){
    for(int j = 0; j < dim.y; j++){
      for(int k = 0; k < dim.z; k++){

        //Generate a new chunk at a specific location
        Chunk chunk;
        chunk.biome = BIOME_VOID;
        chunk.size = chunkSize;
        chunk.pos = glm::vec3(i, j, k);

        //Save the current loaded chunks to file and the world data
        {
          boost::archive::text_oarchive oa(out);
          oa << chunk;    //Append the Chunk to the Region File
        }
      }
    }
  }
}

void World::generateBuildings(){
  //Flat Surface
  blueprint.flatSurface(dim.x*chunkSize, dim.z*chunkSize);
  evaluateBlueprint(blueprint);

  //Add Generated Buildings
  Blueprint _building;
  _building.building(5, RUSTIC);
  blueprint.merge(_building.translate(glm::vec3(100, 1, 100)));
  evaluateBlueprint(blueprint);
}

void World::generateFlat(){
  //Flat Surface
  blueprint.flatSurface(dim.x*chunkSize, dim.z*chunkSize);
  evaluateBlueprint(blueprint);

  //Rocks
  std::cout<<"Adding Clay Boulders"<<std::endl;
  for(int i = 0; i < 1000; i++){
    int rock[2] = {rand()%(chunkSize*(int)dim.x), rand()%(chunkSize*(int)dim.z)};
    blueprint.addEditBuffer(glm::vec3(rock[0], 1, rock[1]), BLOCK_CLAY, false);
  }

  //Evaluate the editBuffer
  evaluateBlueprint(blueprint);

  //Trees
  std::cout<<"Adding Cacti"<<std::endl;
  Blueprint _tree;

  for(int i = 0; i < 2000; i++){
    _tree.editBuffer.clear();
    _tree.cactus(); //Construct a tree blueprint (height = 9
    //Append the Translated Blueprint to the full blueprint.
    int tree[2] = {rand()%(chunkSize*(int)dim.x), rand()%(chunkSize*(int)dim.z)};
    blueprint.merge(_tree.translate(glm::vec3(tree[0], 1, tree[1])));
  }

  //Evaluate the Buffer
  evaluateBlueprint(blueprint);
}

void World::generatePerlin(){
  //Perlin Noise Generator
  noise::module::Perlin perlin;
  perlin.SetOctaveCount(10);
  perlin.SetFrequency(6);
  perlin.SetPersistence(0.5);


  //Adding Water to world.
  std::cout<<"Flooding World."<<std::endl;
  for(int i = 0; i < dim.x*chunkSize; i++){
    for(int j = 0; j < sealevel; j++){
      for(int k = 0; k < dim.z*chunkSize; k++){
        //Add water up to a specific height
        blueprint.addEditBuffer(glm::vec3(i, j, k), BLOCK_WATER, false);
      }
    }
  }
  //Flood
  evaluateBlueprint(blueprint);

  //Loop over the world-size
  std::cout<<"Adding Ground."<<std::endl;
  for(int i = 0; i < dim.x*chunkSize; i++){
    for(int k = 0; k < dim.z*chunkSize; k++){

      //Normalize the Block's x,z coordinates
      float x = (float)(i) / (float)(chunkSize*dim.x);
      float z = (float)(k) / (float)(chunkSize*dim.z);

      float height = perlin.GetValue(x, SEED, z)/5+0.25;
      height *= (dim.y*chunkSize);

      //Now loop over the height and set the blocks
      for(int j = 0; j < (int)height-1; j++){
        if(j < sealevel-1) blueprint.addEditBuffer(glm::vec3(i, j, k), BLOCK_STONE, false);
        else if(j <= sealevel) blueprint.addEditBuffer(glm::vec3(i, j, k), BLOCK_SAND, false);
        else blueprint.addEditBuffer(glm::vec3(i, j, k), BLOCK_DIRT, false);
      }

      //Add Grass on top
      if(height > sealevel+3) blueprint.addEditBuffer(glm::vec3(i, (int)height-1, k), BLOCK_GRASS, false);
    }
  }

  //Evaluate the Guy
  evaluateBlueprint(blueprint);

  //Rocks
  std::cout<<"Adding Rocks"<<std::endl;
  for(int i = 0; i < 500; i++){
    int rock[2] = {rand()%(chunkSize*(int)dim.x), rand()%(chunkSize*(int)dim.z)};
    //Normalize the Block's x,z coordinates
    float x = (float)(rock[0]) / (float)(chunkSize*dim.x);
    float z = (float)(rock[1]) / (float)(chunkSize*dim.z);

    float height = perlin.GetValue(x, SEED, z)/5+0.25;
    height *= (dim.y*chunkSize);

    if(height < sealevel) continue;

    blueprint.addEditBuffer(glm::vec3(rock[0], (int)height, rock[1]), BLOCK_STONE, false);
  }

  //Evaluate the Guy
  evaluateBlueprint(blueprint);

  //Pumpkings
  std::cout<<"Adding Pumpkins"<<std::endl;
  for(int i = 0; i < 1000; i++){
    int rock[2] = {rand()%(chunkSize*(int)dim.x), rand()%(chunkSize*(int)dim.z)};
    //Normalize the Block's x,z coordinates
    float x = (float)(rock[0]) / (float)(chunkSize*dim.x);
    float z = (float)(rock[1]) / (float)(chunkSize*dim.z);

    float height = perlin.GetValue(x, SEED, z)/5+0.25;
    height *= (dim.y*chunkSize);

    if(height < sealevel) continue;

    blueprint.addEditBuffer(glm::vec3(rock[0], (int)height, rock[1]), BLOCK_PUMPKIN, false);
  }

  //Trees
  std::cout<<"Adding Trees"<<std::endl;
  Blueprint _tree;

  for(int i = 0; i < 2500; i++){
    //Generate a random size tree model.
    int treeheight = rand()%6+8;
    _tree.editBuffer.clear();
    _tree.tree(treeheight); //Construct a tree blueprint (height = 9)

    //Append the Translated Blueprint to the full blueprint.
    int tree[2] = {rand()%(chunkSize*(int)dim.x), rand()%(chunkSize*(int)dim.z)};

    float x = (float)(tree[0]) / (float)(chunkSize*dim.x);
    float z = (float)(tree[1]) / (float)(chunkSize*dim.z);

    float height = perlin.GetValue(x, SEED, z)/5+0.25;
    height *= (dim.y*chunkSize);

    if(height < 16) continue;

    blueprint.merge(_tree.translate(glm::vec3(tree[0], (int)height, tree[1])));
  }

  //Evaluate the Buffer
  evaluateBlueprint(blueprint);
}

/*
===================================================
          BLUEPRINT HANDLING FUNCTIONS
===================================================
*/

bool World::evaluateBlueprint(Blueprint &_blueprint){
  //Check if the editBuffer isn't empty!
  if(_blueprint.editBuffer.empty()){
    return false;
  }

  //Sort the editBuffer
  std::sort(_blueprint.editBuffer.begin(), _blueprint.editBuffer.end(), std::greater<bufferObject>());

  //Open the File
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "save";
  data_dir /= saveFile;

  //Load File and Write File
  std::ifstream in((data_dir/"world.region").string());
  std::ofstream out((data_dir/"world.region.temp").string(), std::ofstream::app);

  //Chunk for Saving Data
  Chunk _chunk;
  int n_chunks = 0;

  //Loop over the Guy
  while(n_chunks < dim.x*dim.y*dim.z){
    if(in.eof()){
      return false;
    }

    //Archive Serializers
    boost::archive::text_oarchive oa(out);
    boost::archive::text_iarchive ia(in);

    //Load the Chunk
    ia >> _chunk;

    //Overwrite relevant portions
    while(!_blueprint.editBuffer.empty() && glm::all(glm::equal(_chunk.pos, _blueprint.editBuffer.back().cpos))){
      //Change the Guy
      _chunk.setPosition(glm::mod(_blueprint.editBuffer.back().pos, glm::vec3(chunkSize)), _blueprint.editBuffer.back().type);
      _blueprint.editBuffer.pop_back();
    }

    //Write the chunk back
    oa << _chunk;
    n_chunks++;
  }

  //Close the fstream and ifstream
  in.close();
  out.close();

  //Delete the first file, rename the temp file
  boost::filesystem::remove_all((data_dir/"world.region").string());
  boost::filesystem::rename((data_dir/"world.region.temp").string(),(data_dir/"world.region").string());

  //Success!
  return true;
}

/*
===================================================
          MOVEMENT RELATED FUNCTIONS
===================================================
*/

int World::moveWeight(BlockType _type){
  switch(_type){
    case BLOCK_AIR:
      return 1;
    default:
      return 10;
  }
}

BlockType World::getBlock(glm::vec3 _pos){
  //Chunk Position and World Position
  glm::vec3 c = glm::floor(_pos/glm::vec3(chunkSize));
  glm::vec3 p = glm::mod(_pos, glm::vec3(chunkSize));

  for(unsigned int i = 0; i < chunks.size(); i++){
    //Compare to Chunk Position
    if(c == chunks[i].pos){
      return (BlockType)chunks[i].data[chunks[i].getIndex(p)];
    }
  }
  return BLOCK_AIR;
}

void World::setBlock(glm::vec3 _pos, BlockType _type){
  //Check if the position is inside, if not return 0, otherwise return the block
  for(unsigned int i = 0; i < chunks.size(); i++){
    glm::vec3 c = glm::floor(_pos/glm::vec3(chunkSize));
    //Check the Chunkpos
    if(c == chunks[i].pos){
      chunks[i].setPosition(glm::mod(_pos, glm::vec3(chunkSize)), _type);
      chunks[i].refreshModel = true;
      break;
    }
  }
}

//Get the Top-Free space position in the x-z position
glm::vec3 World::getTop(glm::vec2 _pos){
  //Highest Block you can Stand O
  int max = 0;

  //Loop over the height
  for(int i = 1; i < dim.y*chunkSize; i++){
    //Check if we satisfy the conditions
    if(getBlock(glm::vec3(_pos.x, i, _pos.y)) == BLOCK_AIR && getBlock(glm::vec3(_pos.x, i-1, _pos.y)) == BLOCK_GRASS){
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

void World::bufferChunks(View view){
  //Load / Reload all Visible Chunks
  evaluateBlueprint(blueprint);

  //Chunks that should be loaded
  glm::vec3 a = glm::floor(view.viewPos/glm::vec3(chunkSize))-view.renderDistance;
  glm::vec3 b = glm::floor(view.viewPos/glm::vec3(chunkSize))+view.renderDistance;

  //Can't exceed a certain size
  a = glm::clamp(a, glm::vec3(0), dim-glm::vec3(1));
  b = glm::clamp(b, glm::vec3(0), dim-glm::vec3(1));

  //Chunks that need to be removed
  std::stack<int> remove;
  std::vector<glm::vec3> load;

  //Construct the Vector of guys we should load
  for(int i = a.x; i <= b.x; i ++){
    for(int j = a.y; j <= b.y; j ++){
      for(int k = a.z; k <= b.z; k ++){
        //Add the vector that we should be loading
        load.push_back(glm::vec3(i, j, k));
      }
    }
  }

  //Loop over all existing chunks
  for(unsigned int i = 0; i < chunks.size(); i++){
    //Check if any of these chunks are outside of the limits of a / b
    if(glm::any(glm::lessThan(chunks[i].pos, a)) || glm::any(glm::greaterThan(chunks[i].pos, b))){
      //Add the chunk to the erase pile
      remove.push(i);
    }

    //Make sure that the chunk that we determined will not be removed is also not reloaded
    for(unsigned int j = 0; j < load.size(); j++){
      if(glm::all(glm::equal(load[j], chunks[i].pos))){
        //Remove the element from load, as it is already inside this guy
        load.erase(load.begin()+j);
      }
    }
  }

  updateModels = remove;

  //Loop over the erase pile, delete the relevant chunks.
  while(!remove.empty()){
    chunks.erase(chunks.begin()+remove.top());
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

    boost::filesystem::path data_dir(boost::filesystem::current_path());
    data_dir /= "save";
    data_dir /= saveFile;
    std::ifstream in((data_dir/"world.region").string());

    Chunk _chunk;
    int n = 0;

    while(!load.empty()){
      //Skip Lines
      while(n < load.back().x*dim.z*dim.y+load.back().y*dim.z+load.back().z){
        in.ignore(1000000,'\n');
        n++;
      }
      //Load the Chunk
      {
        boost::archive::text_iarchive ia(in);
        ia >> _chunk;
        chunks.push_back(_chunk);
        load.pop_back();
      }
    }
    in.close();
  }
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
    //Save the world here
    saveWorld();
    return true;
  }

  //Load the World
  data_dir /= "world.meta";
  std::ifstream in(data_dir.string());
  {
    boost::archive::text_iarchive ia(in);
    ia >> *this;
  }
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
  return true;
}


namespace boost {
namespace serialization {

//Chunk Serializer
template<class Archive>
void serialize(Archive & ar, Chunk & _chunk, const unsigned int version)
{
  ar & _chunk.pos.x;
  ar & _chunk.pos.y;
  ar & _chunk.pos.z;
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
  ar & _world.chunkSize;
  ar & _world.dim.x;
  ar & _world.dim.y;
  ar & _world.dim.z;
}

} // namespace serialization
} // namespace boost
