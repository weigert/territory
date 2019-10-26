#include "blueprint.h"

/*
===================================================
                UTILITY FUNCTIONS
===================================================
*/

bool Blueprint::addEditBuffer(glm::vec3 _pos, BlockType _type, bool negative){
  //Check validity
  if(!negative && (glm::any(glm::lessThan(_pos, glm::vec3(0))) || glm::any(glm::greaterThanEqual(_pos, glm::vec3(chunkSize)*dim))))
  return false;

  //Add a bufferObject
  editBuffer.push_back(bufferObject());
  editBuffer.back().pos = _pos;
  editBuffer.back().type = _type;
  editBuffer.back().cpos = glm::floor(_pos/glm::vec3(chunkSize));
  return true;
}

//Append a blueprint to the current blueprint.
void Blueprint::merge(Blueprint _other){
  //Append all buffer objects to another buffer object (unsorted)
  editBuffer.insert(editBuffer.end(), _other.editBuffer.begin(), _other.editBuffer.end());

  //Maybe check for overrides in the future.
}

//Return a translated blueprint version.
Blueprint Blueprint::translate(glm::vec3 _pos){
  Blueprint _new;
  //Loop over all buffer objects
  for(bufferObject& obj : editBuffer){
    _new.addEditBuffer(_pos + obj.pos, obj.type, false);
  }
  return _new;
}

/*
===================================================
                SAVING AND LOADING
===================================================
*/

void Blueprint::save(std::string savefile){
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "resource/blueprint";
  data_dir /= savefile;

  std::ofstream out(data_dir.string());
  {
    boost::archive::text_oarchive oa(out);
    oa << *this;
  }
}

void Blueprint::load(std::string loadfile){
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "resource/blueprint";
  data_dir /= loadfile;
  std::ifstream in(data_dir.string());
  {
    boost::archive::text_iarchive ia(in);
    ia >> *this;
  }
}

/*
===================================================
                  CONSTRUCTORS
===================================================
*/

void Blueprint::flatSurface(int x, int z){
  std::cout<<"Generating Flat Surface"<<std::endl;
  for(int i = 0; i < x; i++){
    for(int j = 0; j < z; j++){
      //Add to the editBuffer
      addEditBuffer(glm::vec3(i,0,j), BLOCK_SAND, false);
    }
  }
}

//Do this guy
void Blueprint::cactus(int height){
  //Loop until the height
  for(int i = 0; i < height; i++){
    addEditBuffer(glm::vec3(0, i, 0), BLOCK_GRASS, true);
  }
}

//Do this guy
void Blueprint::tree(int height){
  //Loop until the height
  for(int i = 0; i < height; i++){
    addEditBuffer(glm::vec3(0, i, 0), BLOCK_WOOD, true);
  }

  //Add Leaf-Crown
  addEditBuffer(glm::vec3(0, height+1, 0), BLOCK_LEAVES, true);
  addEditBuffer(glm::vec3(1, height, 0), BLOCK_LEAVES, true);
  addEditBuffer(glm::vec3(0, height, 1), BLOCK_LEAVES, true);
  addEditBuffer(glm::vec3(-1, height, 0), BLOCK_LEAVES, true);
  addEditBuffer(glm::vec3(0, height, -1), BLOCK_LEAVES, true);
}
