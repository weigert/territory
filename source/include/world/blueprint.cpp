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

void Blueprint::removeDuplicates(bool later){
  //Create a new blueprint...
  Blueprint _new;

  //Keep the ones that came last
  if(later){
    //Loop over the old blueprint size
    for(int i = editBuffer.size()-1; i >= 0; i--){ //We want to keep stuff we added later, so loop over back.
      //Loop over the new editBuffer size!
      bool duplicate = false;
      for(unsigned int j = 0; j < _new.editBuffer.size(); j++){
        //Check if they are identical some other guy
        if(glm::all(glm::equal(editBuffer[i].pos+editBuffer[i].cpos*glm::vec3(16), _new.editBuffer[j].pos+_new.editBuffer[j].cpos*glm::vec3(16)))){
          duplicate = true;
          break;
        }
      }
      //Add the old guy we found to the new blueprint.
      if(!duplicate){
        _new.editBuffer.push_back(editBuffer[i]);
      }
    }
  }
  else{ //Keep the ones that came first!
    //Loop over the old blueprint size
    for(unsigned int i = 0; i <editBuffer.size(); i++){
      //Loop over the new editBuffer size!
      bool duplicate = false;
      for(unsigned int j = 0; j < _new.editBuffer.size(); j++){
        //Check if they are identical some other guy
        if(glm::all(glm::equal(editBuffer[i].pos+editBuffer[i].cpos*glm::vec3(16), _new.editBuffer[j].pos+_new.editBuffer[j].cpos*glm::vec3(16)))){
          duplicate = true;
          break;
        }
      }
      //Add the old guy we found to the new blueprint.
      if(!duplicate){
        _new.editBuffer.push_back(editBuffer[i]);
      }
    }
  }

  //Set this to new
  *this = _new;
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
      addEditBuffer(glm::vec3(i,0,j), BLOCK_GRASS, false);
    }
  }
}

//Do this guy
void Blueprint::cactus(){
  //Loop until the height
  int height = 4+rand()%2;
  for(int i = 0; i < height; i++){
    addEditBuffer(glm::vec3(0, i, 0), BLOCK_CACTUS, true);
  }
}

//Do this guy
void Blueprint::tree(int height){
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


void Blueprint::hut(){
  //Add Floor
  for(int i = 0; i < 5; i++){
    for(int k = 0; k < 5; k++){
      addEditBuffer(glm::vec3(i-2, -1, k-2), BLOCK_STONE, true);
    }
  }

  //Create a hut!
  for(int i = 0; i < 5; i++){
    for(int j = 0; j < 5; j++){
      for(int k = 0; k < 5; k++){
        //Ignore the Door!
        if(i == 2 && k == 0 && (j == 0 || j == 1 || j == 2)){
          addEditBuffer(glm::vec3(i-2, j, k-2), BLOCK_AIR, true);
          continue;
        }
        //Ignore the Window
        if(i == 0 && k == 2 && j == 2){
          addEditBuffer(glm::vec3(i-2, j, k-2), BLOCK_AIR, true);
          continue;
        }
        //The rest is otherwise planks if on the edge
        if(i == 0 || i == 4 || j == 4 || k == 0 || k == 4 ){
          addEditBuffer(glm::vec3(i-2, j, k-2), BLOCK_PLANKS, true);
          continue;
        }
        //And otherwise straight up air.
        addEditBuffer(glm::vec3(i-2, j, k-2), BLOCK_AIR, true);
      }
    }
  }

  //Add some stairs on the side
  addEditBuffer(glm::vec3(3, -1, -2), BLOCK_STONE, true);
  addEditBuffer(glm::vec3(3, 0, -1), BLOCK_STONE, true);
  addEditBuffer(glm::vec3(3, 1, 0), BLOCK_STONE, true);
  addEditBuffer(glm::vec3(3, 2, 1), BLOCK_STONE, true);
  addEditBuffer(glm::vec3(3, 3, 2), BLOCK_STONE, true);
}

//Generate a Building
void Blueprint::building(const int &n, BuildingStyle style){
  std::cout<<"Generating Building"<<std::endl;

  //Seed the Random Generator...
  srand(time(NULL));

  //Generate a Graph
  graph::Graph<graph::ROOMGRAPH> _roomgraph;
  _roomgraph.generate(n);
  _roomgraph.optimize();

  buildingFromGraph(_roomgraph, style);
}

void Blueprint::buildingFromGraph(graph::Graph<graph::ROOMGRAPH> rooms, BuildingStyle style){

  BlockType surface;
  BlockType pillar;
  BlockType floor;
  BlockType roof;

  if(style == RUSTIC){
    surface = BLOCK_PLANKS;
    pillar = BLOCK_WOOD;
    floor = BLOCK_STONE;
    roof = BLOCK_CLAY;
  }
  if(style == CASTLE){
    surface = BLOCK_STONE;
    pillar = BLOCK_STONE;
    floor = BLOCK_STONE;
    roof = BLOCK_STONE;
  }

  editBuffer.clear();

  if(style == RUSTIC){
    //Finally, construct the actualy building.
    for(unsigned int r = 1; r < rooms.nodes.size(); r++){
      //Construct the Volume for every room!

      //Construct the Volumes
      for(int i = rooms.nodes[r].volume.a.x; i <=rooms.nodes[r].volume.b.x; i++){
        for(int j = rooms.nodes[r].volume.a.y; j <=rooms.nodes[r].volume.b.y; j++){
          for(int k = rooms.nodes[r].volume.a.z; k <=rooms.nodes[r].volume.b.z; k++){
            if(i == rooms.nodes[r].volume.a.x || i == rooms.nodes[r].volume.b.x) addEditBuffer(glm::vec3(i, j, k), surface, true);
            else if(j == rooms.nodes[r].volume.a.y || j == rooms.nodes[r].volume.b.y) addEditBuffer(glm::vec3(i, j, k), surface, true);
            else if(k == rooms.nodes[r].volume.a.z || k == rooms.nodes[r].volume.b.z) addEditBuffer(glm::vec3(i, j, k), surface, true);
            else addEditBuffer(glm::vec3(i, j, k), BLOCK_AIR, true);
          }
        }
      }
    }
  }
  else if(style == CASTLE){
    //Finally, construct the actualy building.
    for(unsigned int r = 1; r < rooms.nodes.size(); r++){
      //Construct the Volume for every room!

      //Construct the Volumes
      for(int i = rooms.nodes[r].volume.a.x; i <=rooms.nodes[r].volume.b.x; i++){
        for(int j = rooms.nodes[r].volume.a.y; j <=rooms.nodes[r].volume.b.y; j++){
          for(int k = rooms.nodes[r].volume.a.z; k <=rooms.nodes[r].volume.b.z; k++){
            if(i == rooms.nodes[r].volume.a.x || i == rooms.nodes[r].volume.b.x) addEditBuffer(glm::vec3(i, j, k), surface, true);
            else if(j == rooms.nodes[r].volume.a.y || j == rooms.nodes[r].volume.b.y) addEditBuffer(glm::vec3(i, j, k), surface, true);
            else if(k == rooms.nodes[r].volume.a.z || k == rooms.nodes[r].volume.b.z) addEditBuffer(glm::vec3(i, j, k), surface, true);
            else addEditBuffer(glm::vec3(i, j, k), BLOCK_AIR, true);
          }
        }
      }
    }
  }


  for(unsigned int r = 1; r < rooms.nodes.size(); r++){
    //Create the Pillars!
    if(rooms.nodes[r].volume.b.y > 0){
      int n = 0;
      while(rooms.nodes[r].volume.b.y-n >= 0){
        //Four Corners!!
        addEditBuffer(glm::vec3(rooms.nodes[r].volume.a.x, rooms.nodes[r].volume.b.y, rooms.nodes[r].volume.a.z)+glm::vec3(0, -n, 0), pillar, true);
        addEditBuffer(glm::vec3(rooms.nodes[r].volume.b.x, rooms.nodes[r].volume.b.y, rooms.nodes[r].volume.a.z)+glm::vec3(0, -n, 0), pillar, true);
        addEditBuffer(glm::vec3(rooms.nodes[r].volume.a.x, rooms.nodes[r].volume.b.y, rooms.nodes[r].volume.b.z)+glm::vec3(0, -n, 0), pillar, true);
        addEditBuffer(glm::vec3(rooms.nodes[r].volume.b.x, rooms.nodes[r].volume.b.y, rooms.nodes[r].volume.b.z)+glm::vec3(0, -n, 0), pillar, true);
        n++;
      }
    }
  }

  //Generate a Floor for every room!
  for(unsigned int r = 1; r < rooms.nodes.size(); r++){
    //Add floors!
    for(int i = rooms.nodes[r].volume.a.x; i <=rooms.nodes[r].volume.b.x; i++){
      for(int k = rooms.nodes[r].volume.a.z; k <=rooms.nodes[r].volume.b.z; k++){
        addEditBuffer(glm::vec3(i, rooms.nodes[r].volume.a.y, k), floor, true);
      }
    }
  }

  //Remove the Duplicate Buffer entries, keep what was placed later!
  removeDuplicates(true);

  if(style == RUSTIC){
    //Generate a Roof for every guy
    for(unsigned int r = 1; r < rooms.nodes.size(); r++){

      //Loop over the x-coordinate
      for(int i = rooms.nodes[r].volume.a.x; i <=rooms.nodes[r].volume.b.x; i++){

        //Loop over the z-coordinate
        for(int k = rooms.nodes[r].volume.a.z; k <=rooms.nodes[r].volume.b.z; k++){

          int height1 = k-rooms.nodes[r].volume.a.z;
          int height2 = rooms.nodes[r].volume.b.z-k;
          int height = (height1 < height2)?height1:height2;

          while(height >= 0){
            //Place the height in dependency of Z!
            addEditBuffer(glm::vec3(i, rooms.nodes[r].volume.b.y+height+1, k), roof, true);
            height--;
          }

        }
      }
    }
  }
  else if(style == CASTLE){
    //Generate a Roof for every guy
    for(unsigned int r = 1; r < rooms.nodes.size(); r++){

      //Loop over the x-coordinate
      for(int i = rooms.nodes[r].volume.a.x; i <=rooms.nodes[r].volume.b.x; i++){
        for(int k = rooms.nodes[r].volume.a.z; k <=rooms.nodes[r].volume.b.z; k++){
          if(i != rooms.nodes[r].volume.a.x && i != rooms.nodes[r].volume.b.x && k != rooms.nodes[r].volume.a.z && k != rooms.nodes[r].volume.b.z) continue;
          addEditBuffer(glm::vec3(i, rooms.nodes[r].volume.b.y+1, k), roof, true);
        }
      }
    }
  }

  //Remove the duplicates, but keep whatever was placed earlier!
  removeDuplicates(false);
}
