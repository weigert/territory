//Include our Class Definition
//Include Dependencies
#include "../world/world.h"
#include "../render/view.h"
#include "player.h"

#include "event.h"

void eventHandler::input(SDL_Event *e, bool &quit, bool &paused){
  //User input detected
  if( SDL_PollEvent( e ) != 0 ) {
    //Quit Event
    if( e->type == SDL_QUIT ) { quit = true; }
    //Key Down Top-Event
    else if( e->type == SDL_KEYDOWN ) {
      if(e->key.keysym.sym == SDLK_UP  && rotate.empty()){
        rotate.push_front(e);
      }
      else if(e->key.keysym.sym == SDLK_DOWN  && rotate.empty()){
        rotate.push_front(e);
      }
      else if(e->key.keysym.sym == SDLK_p){
        //Invert the Pause State
        paused = !paused;
      }
      else{
        inputs.push_front(e);
      }
    }
    else if( e->type == SDL_MOUSEWHEEL){
      scroll.push_front(e);
    }
    else if( e->type == SDL_MOUSEBUTTONDOWN){
      mouse = e;
      click = true;
    }
    else if( e->type == SDL_MOUSEMOTION){
      mouse = e;
      move = true;
    }
  }
}

void eventHandler::update(World &world, Player &player, Population &population, View &view){
  //Do this thingy to get the current mouse position that is being thing'd
  if(click){
    glm::vec2 pos = glm::vec2(mouse->button.x, mouse->button.y);
    if(mouse->button.button == SDL_BUTTON_LEFT){
      view.focus = glm::vec2(1.0)-pos/glm::vec2(view.SCREEN_WIDTH, view.SCREEN_HEIGHT); //Set current focus
    }
    else if(mouse->button.button == SDL_BUTTON_RIGHT){
      view.select = view.intersect(world, glm::vec2(mouse->button.x, mouse->button.y));
      view.picked = true;
    }
    click = false;
  }
  if(move){
    move = false;
  }

  //Check for rotation
  if(!inputs.empty()){
    //Handle the Player Move
    if(inputs.front()->key.keysym.sym == SDLK_w){
      handlePlayerMove(world, player, view, 0);
    }
    else if(inputs.front()->key.keysym.sym == SDLK_a){
      handlePlayerMove(world, player, view, 1);
    }
    else if(inputs.front()->key.keysym.sym == SDLK_s){
      handlePlayerMove(world, player, view, 2);
    }
    else if(inputs.front()->key.keysym.sym == SDLK_d){
      handlePlayerMove(world, player, view, 3);
    }
    else if(inputs.front()->key.keysym.sym == SDLK_SPACE){
      handlePlayerMove(world, player, view, 4);
    }
    else if(inputs.front()->key.keysym.sym == SDLK_LSHIFT){
      handlePlayerMove(world, player, view, 5);
    }
    else if(inputs.front()->key.keysym.sym == SDLK_m){
    }
    //Remove the command
    inputs.pop_back();
  }

  if(!rotate.empty()){
    //We want to perform an event now..
    if(rotate.front()->key.keysym.sym == SDLK_UP){
      glm::vec3 axis(0.0f, 1.0f, 0.0f);
      view.camera = glm::lookAt(glm::vec3(10,12,10), glm::vec3(0,2,0), glm::vec3(0,1,0));
      view.camera = glm::rotate(view.camera, glm::radians(view.rotation), axis);
      rotate.pop_back();
    }
    else if(rotate.back()->key.keysym.sym == SDLK_DOWN){
      glm::vec3 axis(0.0f, 1.0f, 0.0f);
      view.camera = glm::lookAt(glm::vec3(10,2,10), glm::vec3(0,2,0), glm::vec3(0,1,0));
      view.camera = glm::rotate(view.camera, glm::radians(view.rotation), axis);
      rotate.pop_back();
    }
    else{
      rotate.pop_back();
    }
  }

  if(!scroll.empty()){
    //Scroll Away
    if(scroll.front()->wheel.y > 0.99 && view.zoom <= 0.3){
      //Change the Zoom Value and Projection Matrix
      view.zoom+=view.zoomInc;
      view.projection = glm::ortho(-(float)view.SCREEN_WIDTH*view.zoom, (float)view.SCREEN_WIDTH*view.zoom, -(float)view.SCREEN_HEIGHT*view.zoom, (float)view.SCREEN_HEIGHT*view.zoom, -100.0f, 100.0f);
      scroll.pop_back();
    }
    //Scroll Closer
    else if(scroll.back()->wheel.y < -0.99 && view.zoom > 0.005){
      view.zoom-=view.zoomInc;
      view.projection = glm::ortho(-(float)view.SCREEN_WIDTH*view.zoom, (float)view.SCREEN_WIDTH*view.zoom, -(float)view.SCREEN_HEIGHT*view.zoom, (float)view.SCREEN_HEIGHT*view.zoom, -100.0f, 100.0f);
      scroll.pop_back();
    }
    else if(scroll.back()->wheel.x < -0.8){
      glm::vec3 axis(0.0f, 1.0f, 0.0f);
      view.rotation += 1.5f;
      view.camera = glm::rotate(view.camera, glm::radians(1.5f), axis);
      //view.sprite.model = glm::rotate(view.sprite.model, glm::radians(-1.5f), axis);
      scroll.pop_back();
    }
    else if(scroll.back()->wheel.x > 0.8){
      glm::vec3 axis(0.0f, 1.0f, 0.0f);
      view.rotation -= 1.5f;
      view.camera = glm::rotate(view.camera, glm::radians(-1.5f), axis);
      //view.sprite.model = glm::rotate(view.sprite.model, glm::radians(1.5f), axis);
      scroll.pop_back();
    }
  }
}

void eventHandler::handlePlayerMove(World &world, Player &player, View &view, int a){
  //Movement Vector
  glm::vec3 m;

  //Get the type of movement
  switch(a){
    //W
    case 0:
      m = glm::vec3(0,0,-1);
      break;
    //A
    case 1:
      m = glm::vec3(-1,0,0);
      break;
    //S
    case 2:
      m = glm::vec3(0,0,1);
      break;
    //D
    case 3:
      m = glm::vec3(1,0,0);
      break;
    case 4:
      m = glm::vec3(0,1,0);
      break;
    case 5:
      m = glm::vec3(0,-1,0);
      break;
  }

  //Check if chunks need to be reloaded
  if(glm::any(glm::lessThan(glm::mod(view.viewPos, glm::vec3(world.chunkSize))+m, glm::vec3(0))) || glm::any(glm::greaterThanEqual(glm::mod(view.viewPos, glm::vec3(world.chunkSize))+m, glm::vec3(world.chunkSize)))){
    //Do the thing
    view.viewPos += m;
    world.bufferChunks( view );
    view.loadChunkModels(world); //This can be made more efficient to only reload new chunks
  }

  //Regularly shift player's position
  else if(glm::all(glm::greaterThan(view.viewPos+m, glm::vec3(0))) && glm::all(glm::lessThan(view.viewPos+m, glm::vec3(world.chunkSize)*world.dim-glm::vec3(1)))){
    //Shift the Player's Position
    view.viewPos += m;
    for(unsigned int i = 0; i < view.models.size(); i++){
      view.models[i].translate(-m);
    }
  }
}
