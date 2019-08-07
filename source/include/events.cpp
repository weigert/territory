#include "events.h"

void eventHandler::input(SDL_Event *e, bool &quit){
  //User input detected
  if( SDL_PollEvent( e ) != 0 ) {
    //Quit Event
    if( e->type == SDL_QUIT ) { quit = true; }
    //Key Down Top-Event
    else if( e->type == SDL_KEYDOWN ) {
      //Rotate Events
      if(e->key.keysym.sym == SDLK_LEFT){
        for(int i = 0; i < 10; i ++){
          rotate.push_back(e);
        }
      }
      else if(e->key.keysym.sym == SDLK_RIGHT){
        for(int i = 0; i < 10; i ++){
          rotate.push_back(e);
        }
      }
      else if(e->key.keysym.sym == SDLK_UP){
        rotate.push_back(e);
      }
      else if(e->key.keysym.sym == SDLK_DOWN){
        rotate.push_back(e);
      }
      //Any other type of event
      else{
        //Add the other key press to inputs
        inputs.push_front(e);
      }
    }
    else if( e->type == SDL_MOUSEWHEEL){
      scroll.push_front(e);
    }
  }
}

void eventHandler::handle(World &world, View &view){
  //Check for rotation
  if(!inputs.empty()){
    //Handle the input at the back
    if(inputs.back()->key.keysym.sym == SDLK_w){
      if(world.playerPos.z > 0){
        world.playerPos.z--;
        glm::vec3 axis = glm::vec3(0,0,1);
        for(unsigned int i = 0; i < view.models.size(); i++){
          view.models[i].translate(axis);
        }
      }
      else if(world.chunkPos.z > 0){
        world.chunkPos.z--; //Increment Chunkposition
        world.playerPos.z = world.chunkSize-1; //Reset Playerposition
        //Load new chunks
        world.loadChunks();
        view.loadChunkModels(world); //This can be made more efficient to only reload new chunks
      }
    }
    else if(inputs.back()->key.keysym.sym == SDLK_d){
      //Increment the Players Position
      if(world.playerPos.x < world.chunkSize-1){
        world.playerPos.x++;
        glm::vec3 axis = glm::vec3(-1,0,0);
        for(unsigned int i = 0; i < view.models.size(); i++){
          view.models[i].translate(axis);
        }
      }
      else if(world.chunkPos.x < world.worldSize -1){
        world.chunkPos.x++;
        world.playerPos.x = 0;
        //Load new chunks
        world.loadChunks();
        view.loadChunkModels(world); //This can be made more efficient to only reload new chunks
      }
    }
    else if(inputs.back()->key.keysym.sym == SDLK_s){
      if(world.playerPos.z < world.chunkSize-1){
        world.playerPos.z++;
        glm::vec3 axis = glm::vec3(0,0,-1);
        for(unsigned int i = 0; i < view.models.size(); i++){
          view.models[i].translate(axis);
        }
      }
      else if(world.chunkPos.z < world.worldSize -1){
        world.chunkPos.z++;
        world.playerPos.z = 0;
        //Load new chunks
        world.loadChunks();
        view.loadChunkModels(world); //This can be made more efficient to only reload new chunks
      }
    }
    else if(inputs.back()->key.keysym.sym == SDLK_a){
      //Check for move legality
      if(world.playerPos.x > 0){
        world.playerPos.x--;
        glm::vec3 axis = glm::vec3(1,0,0);
        for(unsigned int i = 0; i < view.models.size(); i++){
          view.models[i].translate(axis);
        }
      }
      else if(world.chunkPos.x > 0){
        world.chunkPos.x--;
        world.playerPos.x = world.chunkSize-1;
        world.loadChunks();
        view.loadChunkModels(world); //This can be made more efficient to only reload new chunks
      }
    }
    //Remove the command
    inputs.pop_back();
  }

  if(!rotate.empty()){
    //We want to perform an event now..
    if(rotate.back()->key.keysym.sym == SDLK_RIGHT){
      glm::vec3 axis(0.0f, 1.0f, 0.0f);
      view.rotation += 1.5f;
      view.camera = glm::rotate(view.camera, glm::radians(1.5f), axis);
      view.sprite.model = glm::rotate(view.sprite.model, glm::radians(-1.5f), axis);
      rotate.pop_back();
    }
    else if(rotate.back()->key.keysym.sym == SDLK_LEFT){
      glm::vec3 axis(0.0f, 1.0f, 0.0f);
      view.rotation -= 1.5f;
      view.camera = glm::rotate(view.camera, glm::radians(-1.5f), axis);
      view.sprite.model = glm::rotate(view.sprite.model, glm::radians(1.5f), axis);
      rotate.pop_back();
    }
    else if(rotate.back()->key.keysym.sym == SDLK_UP){
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
    if(scroll.back()->wheel.y > 0 && view.zoom < 0.1){
      view.zoom+=0.001;
      view.projection = glm::ortho(-(float)view.SCREEN_WIDTH*view.zoom, (float)view.SCREEN_WIDTH*view.zoom, -(float)view.SCREEN_HEIGHT*view.zoom, (float)view.SCREEN_HEIGHT*view.zoom, -100.0f, 100.0f);
    }
    else if(scroll.back()->wheel.y < 0 && view.zoom > 0.005){
      view.zoom-=0.001;
      view.projection = glm::ortho(-(float)view.SCREEN_WIDTH*view.zoom, (float)view.SCREEN_WIDTH*view.zoom, -(float)view.SCREEN_HEIGHT*view.zoom, (float)view.SCREEN_HEIGHT*view.zoom, -100.0f, 100.0f);
    }
    scroll.pop_back();
  }
}
