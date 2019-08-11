#include "events.h"

void eventHandler::input(SDL_Event *e, bool &quit){
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
      else{
        inputs.push_front(e);
      }
    }
    else if( e->type == SDL_MOUSEWHEEL){
      scroll.push_front(e);
    }
  }
}

void eventHandler::update(World &world, View &view){
  //Check for rotation
  if(!inputs.empty()){
    //Handle the Player Move
    if(inputs.front()->key.keysym.sym == SDLK_w){
      handlePlayerMove(world, view, 0);
    }
    else if(inputs.front()->key.keysym.sym == SDLK_a){
      handlePlayerMove(world, view, 1);
    }
    else if(inputs.front()->key.keysym.sym == SDLK_s){
      handlePlayerMove(world, view, 2);
    }
    else if(inputs.front()->key.keysym.sym == SDLK_d){
      handlePlayerMove(world, view, 3);
    }
    else if(inputs.front()->key.keysym.sym == SDLK_SPACE){
      handlePlayerMove(world, view, 4);
    }
    else if(inputs.front()->key.keysym.sym == SDLK_LSHIFT){
      handlePlayerMove(world, view, 5);
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

      //LOD Change Here
      if((int)(view.zoom*1000) == 50){
        view.LOD -= 1;
        view.updateLOD = true;
        world.renderDistance += glm::vec3(1,1,1);
        world.bufferChunks();
        view.loadChunkModels(world);
      }
      //LOD Change Here
      else if((int)(view.zoom*1000) == 100){
        view.LOD -= 1;
        view.updateLOD = true;
        world.renderDistance += glm::vec3(1,1,1);
        world.bufferChunks();
        view.loadChunkModels(world);
      }
      //And Again
      else if((int)(view.zoom*1000) == 200){
        view.LOD = 1;
        view.updateLOD = true;
        view.loadChunkModels(world);
      }
      scroll.pop_back();
    }
    //Scroll Closer
    else if(scroll.back()->wheel.y < -0.99 && view.zoom > 0.005){
      view.zoom-=view.zoomInc;
      view.projection = glm::ortho(-(float)view.SCREEN_WIDTH*view.zoom, (float)view.SCREEN_WIDTH*view.zoom, -(float)view.SCREEN_HEIGHT*view.zoom, (float)view.SCREEN_HEIGHT*view.zoom, -100.0f, 100.0f);

      //LOD Change Here
      if((int)(view.zoom*1000) == 50){
        view.LOD += 1;
        view.updateLOD = true;
        world.renderDistance -= glm::vec3(1,1,1);
        world.bufferChunks();
        view.loadChunkModels(world);
      }
      //LOD Change Here
      else if((int)(view.zoom*1000) == 99){
        view.LOD += 1;
        view.updateLOD = true;
        world.renderDistance -= glm::vec3(1,1,1);
        world.bufferChunks();
        view.loadChunkModels(world);
      }
      //LOD Change Here
      else if((int)(view.zoom*1000) == 199){
        view.LOD = 2;
        view.updateLOD = true;
        view.loadChunkModels(world);
      }
      scroll.pop_back();
    }
    else if(scroll.back()->wheel.x < -0.8){
      glm::vec3 axis(0.0f, 1.0f, 0.0f);
      view.rotation += 1.5f;
      view.camera = glm::rotate(view.camera, glm::radians(1.5f), axis);
      view.sprite.model = glm::rotate(view.sprite.model, glm::radians(-1.5f), axis);
      scroll.pop_back();
    }
    else if(scroll.back()->wheel.x > 0.8){
      glm::vec3 axis(0.0f, 1.0f, 0.0f);
      view.rotation -= 1.5f;
      view.camera = glm::rotate(view.camera, glm::radians(-1.5f), axis);
      view.sprite.model = glm::rotate(view.sprite.model, glm::radians(1.5f), axis);
      scroll.pop_back();
    }
  }
}

void eventHandler::handlePlayerMove(World &world, View &view, int a){
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

  //Check if the Player stays in-bounds of Chunk
  if( glm::all(glm::greaterThanEqual(world.playerPos+m, glm::vec3(0))) && glm::all(glm::lessThanEqual(world.playerPos+m, glm::vec3(world.chunkSize-1)))){
    //Shift the Player's Position
    world.playerPos += m;
    for(unsigned int i = 0; i < view.models.size(); i++){
      view.models[i].translate(-m);
    }
  }
  else if(glm::all(glm::greaterThanEqual(world.chunkPos+m, glm::vec3(0))) && glm::all(glm::lessThanEqual(world.chunkPos+m, glm::vec3(world.worldSize-1)))){
    //Shift the Chunk Position
    world.chunkPos += m;
    //Shift the Player Position
    world.playerPos = glm::mod( world.playerPos + glm::vec3(world.chunkSize) + m , glm::vec3(world.chunkSize));
    world.bufferChunks();
    view.loadChunkModels(world); //This can be made more efficient to only reload new chunks
  }
}
