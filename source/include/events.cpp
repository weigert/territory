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
        world.bufferChunks();
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
        world.bufferChunks();
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
        world.bufferChunks();
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
        world.bufferChunks();
        view.loadChunkModels(world); //This can be made more efficient to only reload new chunks
      }
    }
    else if(inputs.back()->key.keysym.sym == SDLK_SPACE){
      if(world.playerPos.y < world.chunkSize-1){
        world.playerPos.y++;
        glm::vec3 axis = glm::vec3(0,-1,0);
        for(unsigned int i = 0; i < view.models.size(); i++){
          view.models[i].translate(axis);
        }
      }
      else if(world.chunkPos.y < world.worldSize -1){
        world.chunkPos.y++;
        world.playerPos.y = 0;
        //Load new chunks
        world.bufferChunks();
        view.loadChunkModels(world); //This can be made more efficient to only reload new chunks
      }
    }
    else if(inputs.back()->key.keysym.sym == SDLK_LSHIFT){
      //Check for move legality
      if(world.playerPos.y > 0){
        world.playerPos.y--;
        glm::vec3 axis = glm::vec3(0,1,0);
        for(unsigned int i = 0; i < view.models.size(); i++){
          view.models[i].translate(axis);
        }
      }
      else if(world.chunkPos.y > 0){
        world.chunkPos.y--;
        world.playerPos.y = world.chunkSize-1;
        world.bufferChunks();
        view.loadChunkModels(world); //This can be made more efficient to only reload new chunks
      }
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
