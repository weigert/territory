#pragma once
#include "../forward/geology.fwd.h"

class Geology {
public:
  //Image
  glm::vec2 d = glm::vec2(50);
  const int size = 50*50;
  int SEED = 0;

  //Fields as Flat Arrays
  float volcanism[50*50] = {0.0f};
  float plates[50*50] = {0.0f};
  int labels[50*50] = {0};
  float height[50*50] = {0.0f};

  float threshold = 0.5; //(i.e. sea-level)

  //Functions
  void initialize();
  void generate();
  void genImage(std::string name, float a[], float b[], float c[], float max, float min);

  //Stuff
  int clusterPlates(int centers[]);
  void calcOverlap(float overlap[], float newplate[], int labels[], int _nplates);
  void doTimeStep();
};
