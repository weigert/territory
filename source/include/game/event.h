//Forward Declarations
#pragma once
#include "../forward/event.fwd.h"
//Depdendencies
#include "../forward/world.fwd.h"
#include "../forward/view.fwd.h"
#include "../forward/player.fwd.h"
#include "../forward/audio.fwd.h"

class eventHandler{
/*
This class handles user input, creates an appropriate stack of activated events and handles them so that user inputs have continuous effect.
*/
public:
  //Queued Inputs
  std::deque<SDL_Event*> inputs; //General Key Inputs
  std::deque<SDL_Event*> scroll; //General Key Inputs
  std::deque<SDL_Event*> rotate; //Rotate Key Inputs
  SDL_Event* mouse; //Whatever the mouse is doing at a moment
  SDL_Event* windowevent; //Whatever the mouse is doing at a moment
  bool _window;
  bool move = false;
  bool click = false;
  bool fullscreen = false;

  //Take inputs and add them to stack
  void input(SDL_Event *e, bool &quit, bool &paused);

  //Handle the existing stack every tick
  void update(World &world, Population &population, View &view, Audio &audio);

  //Handle Individual Types of Events
  void handlePlayerMove(World &world, View &view, int a);
  void handleCameraMove(World &world, View &view);
};
