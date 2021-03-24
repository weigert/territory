/*
================================================================================
                        Blueprint Generating Methods
================================================================================
*/

#include "blueprint.h"

namespace blueprint {

/*
================================================================================
                    Blueprint Evaluation (Global Access)
================================================================================
*/

World* world = NULL;
bool (World::*eval)(Blueprint&) = NULL;
size_t maxedits = 2 << 24;

void evaluate(Blueprint& print){
  if(world == NULL && eval == NULL) return;
  (world->*eval)(print);
}

/*
================================================================================
                            Blueprint Primitives
================================================================================
*/

Blueprint flat(int x, int z, BlockType type = BLOCK_SAND){
  Blueprint print;
  for(int i = 0; i < x; i++)
    for(int j = 0; j < z; j++)
      print.add(vec3(i,0,j), type, true);
  return print;
}

Blueprint fill(vec3 a, vec3 b, BlockType type){
  Blueprint print;
  for(int i = a.x; i <= b.x; i++)
  for(int j = a.y; j <= b.y; j++)
  for(int k = a.z; k <= b.z; k++)
    print.add(glm::vec3(i, j, k), type, true);
  return print;
}

/*
================================================================================
                             Object Blueprints
================================================================================
*/

Blueprint tree(int height){
  Blueprint print;// = new Blueprint;

  for(int i = 0; i < height; i++){
    print.add(glm::vec3(0, i, 0), BLOCK_WOOD, true);
  }

  //Add Leaf-Crown
  print.add(glm::vec3(0, height+1, 0), BLOCK_LEAVES, true);
  print.add(glm::vec3(1, height, 0), BLOCK_LEAVES, true);
  print.add(glm::vec3(0, height, 1), BLOCK_LEAVES, true);
  print.add(glm::vec3(-1, height, 0), BLOCK_LEAVES, true);
  print.add(glm::vec3(0, height, -1), BLOCK_LEAVES, true);

  return print;
}

/*
Blueprint* cactus(){
  Blueprint* print = new Blueprint;

  int height = 4;
  for(int i = 0; i < height; i++)
    print->add(glm::vec3(0, i, 0), BLOCK_CACTUS, true);

  print->add(glm::vec3(0, height, 0), BLOCK_CACTUSFLOWER, true);

  return print;
}


Blueprint* boulder(){
  Blueprint* print = new Blueprint;
  return print;
}
*/

/*
================================================================================
                        Complex Blueprint Constructions
================================================================================
*/

/*
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
          if(i == node.volume.a.x || i == node.volume.b.x) add(glm::vec3(i, j, k), surface, true);
          else if(j == node.volume.a.y || j == node.volume.b.y) add(glm::vec3(i, j, k), surface, true);
          else if(k == node.volume.a.z || k == node.volume.b.z) add(glm::vec3(i, j, k), surface, true);
          else add(glm::vec3(i, j, k), BLOCK_AIR, true);
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
        add(glm::vec3(i, node.volume.a.y, k), floor, true);
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
        add(glm::vec3(node.volume.a.x, node.volume.b.y, node.volume.a.z)+glm::vec3(0, -n, 0), pillar, true);
        add(glm::vec3(node.volume.b.x, node.volume.b.y, node.volume.a.z)+glm::vec3(0, -n, 0), pillar, true);
        add(glm::vec3(node.volume.a.x, node.volume.b.y, node.volume.b.z)+glm::vec3(0, -n, 0), pillar, true);
        add(glm::vec3(node.volume.b.x, node.volume.b.y, node.volume.b.z)+glm::vec3(0, -n, 0), pillar, true);
        n++;
      }
    }
  }

  clean(true);

  for (auto node: rooms.nodes){
    if(node.ID == 0) continue;

    if(rand()%2){
      //Loop over the x-coordinate
      for(int i = node.volume.a.x; i <= node.volume.b.x; i++){
        for(int k = node.volume.a.z; k <= node.volume.b.z; k++){
          if(i != node.volume.a.x && i != node.volume.b.x && k != node.volume.a.z && k != node.volume.b.z) continue;
          add(glm::vec3(i, node.volume.b.y+1, k), surface, true);
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
            add(glm::vec3(i, node.volume.b.y+height+1, k), roof, true);
            height--;
          }
        }
      }
    }
  }

  clean(false);


  //Remove Walls between rooms and add staircases appropriately

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
            add(glm::vec3(i, j, k), BLOCK_AIR, true);
          }
        }
      }
    }
  }

  clean(true);
  return true;
}
*/

/*


bool Blueprint::buildingFromGraph(graph::Graph<graph::ROOMGRAPH> rooms){
  //Clear edits
  edits.clear();

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
      if(pos.x == node.volume.a.x || pos.x == node.volume.b.x) add(pos, surface, true);
      else if(pos.y == node.volume.a.y || pos.y == node.volume.b.y) add(pos, surface, true);
      else if(pos.z == node.volume.a.z || pos.z == node.volume.b.z) add(pos, surface, true);
      else add(pos, BLOCK_AIR, true);
    });
  }

  //Generate a Floor for every room!
  for (auto node: rooms.nodes){
    if(node.ID == 0) continue;
    for(int i = node.volume.a.x; i <= node.volume.b.x; i++){
      for(int k = node.volume.a.z; k <= node.volume.b.z; k++){
        add(glm::vec3(i, node.volume.a.y, k), floor, true);
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
        add(glm::vec3(node.volume.a.x, node.volume.b.y, node.volume.a.z)+glm::vec3(0, -n, 0), pillar, true);
        add(glm::vec3(node.volume.b.x, node.volume.b.y, node.volume.a.z)+glm::vec3(0, -n, 0), pillar, true);
        add(glm::vec3(node.volume.a.x, node.volume.b.y, node.volume.b.z)+glm::vec3(0, -n, 0), pillar, true);
        add(glm::vec3(node.volume.b.x, node.volume.b.y, node.volume.b.z)+glm::vec3(0, -n, 0), pillar, true);
        n++;
      }
    }
  }

  clean(true);

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
      //    if(node.ID == 1) roof = BLOCK_GRAVEL;
      //    if(node.ID == 2) roof = BLOCK_PUMPKIN;
      //    if(node.ID == 3) roof = BLOCK_GRASS;
      //    if(node.ID == 4) roof = BLOCK_CLAY;
      //    if(node.ID == 5) roof = BLOCK_WATER;

          add(glm::vec3(i, node.volume.b.y+height+1, k), roof, true);
          height--;
        }
      }
    }
  }

  //Remove the duplicates, but keep whatever was placed earlier!
  clean(false);


  //Remove Walls between rooms and add staircases appropriately

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

        //This needs to be reduced until it is just a sliver.

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
            add(glm::vec3(i, j, k), BLOCK_AIR, true);
          }
        }
      }
    }
  }


  //Keep the Windows and Doors
  clean(true);

  return true;
}

//Generate a Building
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
  buildingFromGraph(rooms);
}
*/


}
