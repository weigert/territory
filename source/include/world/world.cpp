//Include Header File
//Dependencies
#include "chunk.h"
#include "octree.h"
#include "../game/player.h"
#include "../game/item.h"
#include "../render/shader.h"
#include "../render/billboard.h"
#include "../render/view.h"
#include "geology.h"
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
  generatePerlin();
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

void World::generateFlat(){
  //Flat Surface
  std::cout<<"Generating Flat Surface"<<std::endl;
  for(int i = 0; i < dim.x*chunkSize; i++){
    for(int j = 0; j < dim.z*chunkSize; j++){
      //Add to the editBuffer
      addEditBuffer(glm::vec3(i,0,j), BLOCK_GRASS);
    }
  }

  //Evaluate the editBuffer
  evaluateEditBuffer();

  //Rocks
  std::cout<<"Adding Rocks"<<std::endl;
  for(int i = 0; i < 100; i++){
    int rock[2] = {rand()%(chunkSize*(int)dim.x), rand()%(chunkSize*(int)dim.z)};
    addEditBuffer(glm::vec3(rock[0], 1, rock[1]), BLOCK_STONE);
  }

  //Evaluate the editBuffer
  evaluateEditBuffer();

  //Trees
  std::cout<<"Adding Trees"<<std::endl;
  for(int i = 0; i < 100; i++){
    int tree[2] = {rand()%(chunkSize*(int)dim.x), rand()%(chunkSize*(int)dim.z)};

    //Add the shit to the editbuffer
    addEditBuffer(glm::vec3(tree[0], 1, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 2, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 3, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 4, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 5, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 6, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 7, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 8, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 9, tree[1]), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0]+1, 8, tree[1]), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0]-1, 8, tree[1]), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0], 8, tree[1]+1), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0], 8, tree[1]-1), BLOCK_LEAVES);
  }

  //Evaluate the Buffer
  evaluateEditBuffer();

}

void World::generatePerlin(){
  //Perlin Noise Generator
  noise::module::Perlin perlin;
  perlin.SetOctaveCount(10);
  perlin.SetFrequency(5);
  perlin.SetPersistence(0.5);

  //Loop over the world-size
  std::cout<<"Generating Perlin Surface"<<std::endl;
  for(int i = 0; i < dim.x*chunkSize; i++){
    for(int k = 0; k < dim.z*chunkSize; k++){

      //Generate the Heightvalue

      //Normalize the Block's x,z coordinates
      float x = (float)(i) / (float)(chunkSize*dim.x);
      float z = (float)(k) / (float)(chunkSize*dim.z);

      float height = perlin.GetValue(x, SEED, z)/5+0.25;
      height *= (dim.y*chunkSize);

      //Now loop over the height and set the blocks
      for(int j = 0; j < (int)height-1; j++){
        //Add the block to the editBuffer
        addEditBuffer(glm::vec3(i, j, k), BLOCK_DIRT);
      }
      addEditBuffer(glm::vec3(i, (int)height-1, k), BLOCK_GRASS);
    }
  }

  //Evaluate the Guy
  evaluateEditBuffer();

  //Rocks
  std::cout<<"Adding Rocks"<<std::endl;
  for(int i = 0; i < 100; i++){
    int rock[2] = {rand()%(chunkSize*(int)dim.x), rand()%(chunkSize*(int)dim.z)};
    //Normalize the Block's x,z coordinates
    float x = (float)(rock[0]) / (float)(chunkSize*dim.x);
    float z = (float)(rock[1]) / (float)(chunkSize*dim.z);

    float height = perlin.GetValue(x, SEED, z)/5+0.25;
    height *= (dim.y*chunkSize);

    addEditBuffer(glm::vec3(rock[0], (int)height, rock[1]), BLOCK_STONE);
  }

  //Evaluate the Guy
  evaluateEditBuffer();

  //Pumpkings
  std::cout<<"Adding Pumpkins"<<std::endl;
  for(int i = 0; i < 1000; i++){
    int rock[2] = {rand()%(chunkSize*(int)dim.x), rand()%(chunkSize*(int)dim.z)};
    //Normalize the Block's x,z coordinates
    float x = (float)(rock[0]) / (float)(chunkSize*dim.x);
    float z = (float)(rock[1]) / (float)(chunkSize*dim.z);

    float height = perlin.GetValue(x, SEED, z)/5+0.25;
    height *= (dim.y*chunkSize);

    addEditBuffer(glm::vec3(rock[0], (int)height, rock[1]), BLOCK_PUMPKIN);
  }


  //Add Trees
  std::cout<<"Adding Trees"<<std::endl;
  for(int i = 0; i < 1000; i++){
    int tree[2] = {rand()%(chunkSize*(int)dim.x), rand()%(chunkSize*(int)dim.z)};
    int treeheight = rand()%6+6;

    float x = (float)(tree[0]) / (float)(chunkSize*dim.x);
    float z = (float)(tree[1]) / (float)(chunkSize*dim.z);

    float height = perlin.GetValue(x, SEED, z)/5+0.25;
    height *= (dim.y*chunkSize);

    for(int j = 0; j <= treeheight; j++){
      //Add the shit to the editbuffer
      addEditBuffer(glm::vec3(tree[0], (int)height+j, tree[1]), BLOCK_WOOD);
    }

    addEditBuffer(glm::vec3(tree[0], (int)height+treeheight+1, tree[1]), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0]+1, (int)height+treeheight, tree[1]), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0]-1, (int)height+treeheight, tree[1]), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0], (int)height+treeheight, tree[1]+1), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0], (int)height+treeheight, tree[1]-1), BLOCK_LEAVES);
  }
  //Evaluate the Guy
  evaluateEditBuffer();
}

void World::generateTectonic(){
  //Generate a geology
  std::cout<<"Generating Geology"<<std::endl;
  Geology geology;
  geology.initialize();
	geology.generate();

  //Get the Tectonic Surface
  std::cout<<"Adding to EditBuffer"<<std::endl;
  for(int i = 0; i < dim.x; i++){
    for(int k = 0; k < dim.z; k++){
      //Loop over the height and set the blocks
      float height = geology.height[helper::getIndex(glm::vec2(i, k), glm::vec2(dim.x, dim.z))];
      height *= chunkSize*dim.y*0.9;

      //Add the individual Chunks
      for(int l = 0; l < chunkSize; l++){
        for(int m = 0; m < chunkSize; m++){
          //Set the Grass and Dirt
          for(int j = 0; j < (int)height-1; j++){
            //Add the block to the editBuffer
            addEditBuffer(glm::vec3(i*chunkSize+l, j, k*chunkSize+m), BLOCK_DIRT);
          }
          addEditBuffer(glm::vec3(i*chunkSize+l, (int)height-1, k*chunkSize+m), BLOCK_GRASS);
        }
      }
    }
  }

  //Evaluate the Guy
  std::cout<<"Evaluating EditBuffer"<<std::endl;
  evaluateEditBuffer();
}

/*
===================================================
          OPTIMIZING UTILITY FUNCTIONS
===================================================
*/

bool World::addEditBuffer(glm::vec3 _pos, BlockType _type){
  //Check validity
  if(glm::any(glm::lessThan(_pos, glm::vec3(0))) || glm::any(glm::greaterThanEqual(_pos, glm::vec3(chunkSize)*dim))){
    //Invalid Position
    return false;
  }

  //Add a new bufferObject
  editBuffer.push_back(bufferObject());
  editBuffer.back().pos = _pos;
  editBuffer.back().cpos = glm::floor(_pos/glm::vec3(chunkSize));
  editBuffer.back().type = _type;

  //Push it on
  return true;
}

//Sorting Operator for bufferObjects
bool operator>(const bufferObject& a, const bufferObject& b) {
  if(a.cpos.x > b.cpos.x) return true;
  if(a.cpos.x < b.cpos.x) return false;

  if(a.cpos.y > b.cpos.y) return true;
  if(a.cpos.y < b.cpos.y) return false;

  if(a.cpos.z > b.cpos.z) return true;
  if(a.cpos.z < b.cpos.z) return false;

  return false;
}

bool World::evaluateEditBuffer(){
  //Check if the editBuffer isn't empty!
  if(editBuffer.empty()){
    return false;
  }

  //Sort the editBuffer
  std::sort(editBuffer.begin(), editBuffer.end(), std::greater<bufferObject>());

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
    while(!editBuffer.empty() && glm::all(glm::equal(_chunk.pos, editBuffer.back().cpos))){
      //Change the Guy
      _chunk.setPosition(glm::mod(editBuffer.back().pos, glm::vec3(chunkSize)), editBuffer.back().type);
      editBuffer.pop_back();
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
    case BLOCK_WATER:
      return 5;
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
  evaluateEditBuffer();

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
      if(load[j].x == chunks[i].pos.x && load[j].y == chunks[i].pos.y && load[j].z == chunks[i].pos.z){
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
