//Forward Declarations
#pragma once
#include "../forward/event.fwd.h"
//Depdendencies
#include "../forward/world.fwd.h"
#include "../forward/view.fwd.h"
#include "../forward/player.fwd.h"

class eventHandler{
/*
This class handles user input, creates an appropriate stack of activated events and handles them so that user inputs have continuous effect.
*/
public:
  //Queued Inputs
  std::deque<SDL_Event*> inputs; //General Key Inputs
  std::deque<SDL_Event*> scroll; //General Key Inputs
  std::deque<SDL_Event*> rotate; //Rotate Key Inputs

  //Take inputs and add them to stack
  void input(SDL_Event *e, bool &quit);

  //Handle the existing stack every tick
  void update(World &world, Player &player, Population &population, View &view);

  //Handle Individual Types of Events
  void handlePlayerMove(World &world, Player &player, View &view, int a);
  void handleCameraMove(World &world, View &view);
};
