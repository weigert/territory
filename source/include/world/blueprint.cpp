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
  editBuffer.insert(editBuffer.end(), _other.editBuffer.begin(), _other.editBuffer.end());
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
    for(unsigned int i = 0; i < editBuffer.size(); i++){
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
void Blueprint::cactus(){
  //Loop until the height
  int height = 4;
  for(int i = 0; i < height; i++){
    addEditBuffer(glm::vec3(0, i, 0), BLOCK_CACTUS, true);
  }
  addEditBuffer(glm::vec3(0, height, 0), BLOCK_CACTUSFLOWER, true);
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

/*
================================================================================
                              BUILDING GENERATORS
================================================================================
*/

template <>
bool Blueprint::buildingFromGraph<CASTLE>(graph::Graph<graph::ROOMGRAPH> rooms){

  BlockType surface = BLOCK_PLANKS;
  BlockType pillar = BLOCK_WOOD;
  BlockType floor = BLOCK_STONE;
  BlockType roof = BLOCK_CLAY;

  //Finally, construct the actualy building.
  for (auto node: rooms.nodes){
    if(node.ID == 0) continue;
    //Construct the Volumes
    for(int i = node.volume.a.x; i <= node.volume.b.x; i++){
      for(int j = 0; j <= node.volume.b.y; j++){
        for(int k = node.volume.a.z; k <= node.volume.b.z; k++){
          if(i == node.volume.a.x || i == node.volume.b.x) addEditBuffer(glm::vec3(i, j, k), surface, true);
          else if(j == node.volume.a.y || j == node.volume.b.y) addEditBuffer(glm::vec3(i, j, k), surface, true);
          else if(k == node.volume.a.z || k == node.volume.b.z) addEditBuffer(glm::vec3(i, j, k), surface, true);
          else addEditBuffer(glm::vec3(i, j, k), BLOCK_AIR, true);
        }
      }
    }
  }

  //Generate a Floor for every room!
  for (auto node: rooms.nodes){
    if(node.ID == 0) continue;
    //Add floors!
    for(int i = node.volume.a.x; i <= node.volume.b.x; i++){
      for(int k = node.volume.a.z; k <= node.volume.b.z; k++){
        addEditBuffer(glm::vec3(i, node.volume.a.y, k), floor, true);
      }
    }
  }

  for (auto node: rooms.nodes){
    if(node.ID == 0) continue;
    //Create the Pillars!
    if(node.volume.b.y > 0){
      int n = 0;
      while(node.volume.b.y-n >= 0){
        //Four Corners!!
        addEditBuffer(glm::vec3(node.volume.a.x, node.volume.b.y, node.volume.a.z)+glm::vec3(0, -n, 0), pillar, true);
        addEditBuffer(glm::vec3(node.volume.b.x, node.volume.b.y, node.volume.a.z)+glm::vec3(0, -n, 0), pillar, true);
        addEditBuffer(glm::vec3(node.volume.a.x, node.volume.b.y, node.volume.b.z)+glm::vec3(0, -n, 0), pillar, true);
        addEditBuffer(glm::vec3(node.volume.b.x, node.volume.b.y, node.volume.b.z)+glm::vec3(0, -n, 0), pillar, true);
        n++;
      }
    }
  }

  removeDuplicates(true);

  for (auto node: rooms.nodes){
    if(node.ID == 0) continue;

    if(rand()%2){
      //Loop over the x-coordinate
      for(int i = node.volume.a.x; i <= node.volume.b.x; i++){
        for(int k = node.volume.a.z; k <= node.volume.b.z; k++){
          if(i != node.volume.a.x && i != node.volume.b.x && k != node.volume.a.z && k != node.volume.b.z) continue;
          addEditBuffer(glm::vec3(i, node.volume.b.y+1, k), surface, true);
        }
      }
    }
    else{
      //Loop over the surface area.
      for(int i = node.volume.a.x; i <= node.volume.b.x; i++){
        for(int k = node.volume.a.z; k <= node.volume.b.z; k++){

          //Find Distance from any edge!
          int height1 = k-node.volume.a.z;
          int height2 = node.volume.b.z-k;
          int height3 = i-node.volume.a.x;
          int height4 = node.volume.b.x-i;
          int heighta = (height1 < height2)?height1:height2;
          int heightb = (height3 < height4)?height3:height4;

          //Make the roop sharper
          int height = 2*((heighta < heightb)?heighta:heightb);

          while(height >= 0){
            //Place the height in dependency of Z!
            addEditBuffer(glm::vec3(i, node.volume.b.y+height+1, k), roof, true);
            height--;
          }
        }
      }
    }
  }

  removeDuplicates(false);

  /*
  Remove Walls between rooms and add staircases appropriately
  */
  for (auto [node, _edges] : rooms.edges){
    if(node.ID == 0) continue;
    for (auto edge : _edges){
      //Outside Connection
      if(edge.to->ID == 0) continue;
      //Overlap between this node and all of its neighboring guys...
      glm::vec3 overlap = overlapSize(rooms.nodes[node.ID].volume, rooms.nodes[edge.to->ID].volume);

      //Find the Overlapping Volume
      Volume intersection = intersectionVolume(rooms.nodes[node.ID].volume, rooms.nodes[edge.to->ID].volume);

      //Prune the Intersection Volume
      if(overlap.x == 0){
        intersection.a += glm::vec3(0, 1, 1);
        intersection.b -= glm::vec3(0, 1, 1);
      }
      else if(overlap.y == 0){
        intersection.a += glm::vec3(1, 0, 1);
        intersection.b -= glm::vec3(1, 0, 1);
      }
      else if(overlap.z == 0){
        intersection.a += glm::vec3(1, 1, 0);
        intersection.b -= glm::vec3(1, 1, 0);
      }

      //Remove the Intersection Volume
      for(int i = intersection.a.x; i <= intersection.b.x; i++){
        for(int j = intersection.a.y; j <= intersection.b.y; j++){
          for(int k = intersection.a.z; k <= intersection.b.z; k++){
            addEditBuffer(glm::vec3(i, j, k), BLOCK_AIR, true);
          }
        }
      }
    }
  }

  removeDuplicates(true);
  return true;
}

template <>
bool Blueprint::buildingFromGraph<RUSTIC>(graph::Graph<graph::ROOMGRAPH> rooms){
  //Clear EditBuffer
  editBuffer.clear();

  //Set the Types
  BlockType surface = BLOCK_PLANKS;
  BlockType pillar = BLOCK_WOOD;
  BlockType floor = BLOCK_STONE;
  BlockType roof = BLOCK_CLAY;

  //Finally, construct the actualy building.
  for (auto node: rooms.nodes){
    if(node.ID == 0) continue;
    //Construct the Volumes
    node.volume.loop([this, surface, node](glm::vec3 pos){
      if(pos.x == node.volume.a.x || pos.x == node.volume.b.x) addEditBuffer(pos, surface, true);
      else if(pos.y == node.volume.a.y || pos.y == node.volume.b.y) addEditBuffer(pos, surface, true);
      else if(pos.z == node.volume.a.z || pos.z == node.volume.b.z) addEditBuffer(pos, surface, true);
      else addEditBuffer(pos, BLOCK_AIR, true);
    });
  }

  //Generate a Floor for every room!
  for (auto node: rooms.nodes){
    if(node.ID == 0) continue;
    for(int i = node.volume.a.x; i <= node.volume.b.x; i++){
      for(int k = node.volume.a.z; k <= node.volume.b.z; k++){
        addEditBuffer(glm::vec3(i, node.volume.a.y, k), floor, true);
      }
    }
  }

  for (auto node: rooms.nodes){
    if(node.ID == 0) continue;
    //Create the Pillars!
    if(node.volume.b.y > 0){
      int n = 0;
      while(node.volume.b.y-n >= 0){
        //Four Corners!!
        addEditBuffer(glm::vec3(node.volume.a.x, node.volume.b.y, node.volume.a.z)+glm::vec3(0, -n, 0), pillar, true);
        addEditBuffer(glm::vec3(node.volume.b.x, node.volume.b.y, node.volume.a.z)+glm::vec3(0, -n, 0), pillar, true);
        addEditBuffer(glm::vec3(node.volume.a.x, node.volume.b.y, node.volume.b.z)+glm::vec3(0, -n, 0), pillar, true);
        addEditBuffer(glm::vec3(node.volume.b.x, node.volume.b.y, node.volume.b.z)+glm::vec3(0, -n, 0), pillar, true);
        n++;
      }
    }
  }

  removeDuplicates(true);

  //Generate a Roof for every guy
  for (auto node: rooms.nodes){
    if(node.ID == 0) continue;

    for(int i = node.volume.a.x; i <= node.volume.b.x; i++){
      for(int k = node.volume.a.z; k <= node.volume.b.z; k++){

        int height1 = k-node.volume.a.z;
        int height2 = node.volume.b.z-k;
        int height = (height1 < height2)?height1:height2;

        while(height >= 0){
          //Place the height in dependency of Z!
      /*    if(node.ID == 1) roof = BLOCK_GRAVEL;
          if(node.ID == 2) roof = BLOCK_PUMPKIN;
          if(node.ID == 3) roof = BLOCK_GRASS;
          if(node.ID == 4) roof = BLOCK_CLAY;
          if(node.ID == 5) roof = BLOCK_WATER;
      */
          addEditBuffer(glm::vec3(i, node.volume.b.y+height+1, k), roof, true);
          height--;
        }
      }
    }
  }

  //Remove the duplicates, but keep whatever was placed earlier!
  removeDuplicates(false);

  /*
  Remove Walls between rooms and add staircases appropriately
  */
  for (auto [node, _edges] : rooms.edges){
    if(node.ID == 0) continue;
    for (auto edge : _edges){
      //Outside Connection
      if(edge.to->ID == 0) continue;
      //Overlap between this node and all of its neighboring guys...
      glm::vec3 overlap = overlapSize(rooms.nodes[node.ID].volume, rooms.nodes[edge.to->ID].volume);

      //Find the Overlapping Volume
      Volume intersection = intersectionVolume(rooms.nodes[node.ID].volume, rooms.nodes[edge.to->ID].volume);

      //Prune the Intersection Volume
      if(overlap.x == 0){
        intersection.a += glm::vec3(0, 1, 1);
        intersection.b -= glm::vec3(0, 1, 1);
      }
      else if(overlap.y == 0){
        /*
        This needs to be reduced until it is just a sliver.
        */
        intersection.a += glm::vec3(1, 0, 1);
        intersection.b -= glm::vec3(1, 0, 1);

      }
      else if(overlap.z == 0){
        intersection.a += glm::vec3(1, 1, 0);
        intersection.b -= glm::vec3(1, 1, 0);
      }

      //Remove the Intersection Volume
      for(int i = intersection.a.x; i <= intersection.b.x; i++){
        for(int j = intersection.a.y; j <= intersection.b.y; j++){
          for(int k = intersection.a.z; k <= intersection.b.z; k++){
            addEditBuffer(glm::vec3(i, j, k), BLOCK_AIR, true);
          }
        }
      }
    }
  }


  //Keep the Windows and Doors
  removeDuplicates(true);

  return true;
}

//Generate a Building
template <BuildingStyle S>
void Blueprint::building(const int &n){
  std::cout<<"Generating Building."<<std::endl;

  //Seed the Random Generator...
  srand(time(NULL));

  //Generate a Graph
  graph::Graph<graph::ROOMGRAPH> rooms;
  rooms.generate(n);
  rooms.optimize();

  //If we can't construct the building from the graph!
  std::cout<<"Performing Sanity Check!"<<std::endl;
  while(!rooms.sanitycheck()){
    std::cout<<"Sanity Check Failed. Retrying..."<<std::endl;
    rooms.generate(n);
    rooms.optimize();
  }
  std::cout<<"Passed Sanity Check!"<<std::endl;

  //Generate Building from Rooms
  buildingFromGraph<S>(rooms);
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
