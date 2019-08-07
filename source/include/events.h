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
  void handle(World &world, View &view);
};
