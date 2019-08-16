#pragma once
#include "../forward/sprite.fwd.h"
//Dependencies


class Sprite{
public:
  //Draw Stuff
  GLuint vao[1];
  GLuint vbo[2];
  GLuint texture;
  glm::mat4 model = glm::mat4(1.0f);

  //Load the Texture File
  bool loadImage(std::string fileName);
  void resetModel();
  void setupBuffer();

  //Animation Stuff
  int frame;
  int animationID;
  bool doAnimationFrame();
  void setAnimation(int id);

  void cleanup();
};


/*
Animation Capabilities:
-> Sprites can handle multiple animations per owner of the sprite!
-> Animations can have varying numbers of frames.
-> Time between frames is constant for every animation
-> The call to do an animation frame will return true only once the animation is complete, so we can use that to wait!
-> Sprites can have a static "animation", which is either an idle position or a single frame, and which immediately return true.
-> How do we handle movement inbetween frames?
  -> When altering the VBO objects, we need to consider if animations include translation or rotation or scaling of the player.
  -> Therefore, an animation should be associated with an iterative movement matrix that we can multiply with the model every animation frame!

3 Different Things can have animations:
  -> NPCs and Animals (Bots)
  -> Objects in the World
  -> The Player

Bots:
  -> Bots iteratively go through a list of tasks which they perform. These tasks should have animations with them.
  -> When a task is called for the first time, it sets the animation that it would like to have.
  -> Before a task is called, it checks if "do animation frame" return true, meaning that it has done the "appeareance" of doing a single step task
  -> Therefore, primitive tasks will be coupled to animations, and if they are performed multiple times, the visual will be performed multiple times

  if(doAnimationFrame()){
    //Perform the next task
    //Which will set a new Animation
    //If the new animation doesn't take any time, then doAnimationFrame above will return true, and the next task list element will be executed.
  }

Player:
  -> Players enter inputs into the keyboard, which enter into a list of inputs they chose.
  -> Every tick, when rendering of the player's sprite, do "doAnimationFrame"
  -> Doing an input actually set's the animation, so that when doAnimationFrame returns false
  -> if doAnimationFrame returns true, it sets the animation to the idle animation (i.e. repeat)


Other things:
  -> Similar solutions
//

Functions:
bool animating;

->setAnimation(id);
  Chooses an animation from a list according to ID
  Somehow knows how long the animation is (something with the image)

->doAnimationFrame();
  -> Returns true if the animation is default, i.e. no animation
  -> Increments the frame number based on the modulo of the tick number
    -> If the frameNumber exceeds the number of frames in the animation, returns true
    -> Updates the VBO for the sprite texture if this is incremented
  -> Returns false, i.e. animation is not complete
*/
