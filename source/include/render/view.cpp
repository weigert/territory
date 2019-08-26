//Load Dependencies
#include "../world/world.h"
#include "sprite.h"
#include "shader.h"
#include "model.h"
#include "../taskbot/population.h"
#include "../taskbot/bot.h"
#include "../game/player.h"

//Load our Own Type!
#include "view.h"

bool View::Init(){
  //Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
    printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    return false;
  }
  //Initialize SDL_TTF
	TTF_Init();

  //Initialize the Window and Context
  gWindow = SDL_CreateWindow( "Territory", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
  if( gWindow == NULL ){
    printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
    return false;
  }
  gContext = SDL_GL_CreateContext(	gWindow );

  //Initialize OPENGL Stuff
	SDL_GL_SetSwapInterval(1);
	glewExperimental = GL_TRUE;
	glewInit();

  //Configure Global OpenGL State
  glEnable( GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
  glEnable(GL_BLEND) ;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);

  //Setup Shaders
  setupShaders();
  setupShadow();

  return true;
}

void View::setupShaders(){
  //Setup Cube Shader
  cubeShader.setup("default.vs", "default.fs");
  cubeShader.addAttribute(0, "in_Position");
  cubeShader.addAttribute(1, "in_Color");
  cubeShader.addAttribute(2, "in_Normal");

  //Setup Depthshader
  depthShader.setup("rendershadow.vs", "rendershadow.fs");
  depthShader.addAttribute(0, "in_Position");

  //Setup Debugshader
  debugShader.setup("debug.vs", "debug.fs");
  debugShader.addAttribute(0, "in_Quad");
  debugShader.addAttribute(1, "in_Tex");

  //Setup Spriteshader
  spriteShader.setup("sprite.vs", "sprite.fs");
  debugShader.addAttribute(0, "in_Quad");
  debugShader.addAttribute(1, "in_Tex");

}

void View::cleanup(){
  //Cleanup Models
  for(unsigned int i = 0; i < models.size(); i++){
    //Cleanup the Models
    models[i].cleanup();
  }

  //Cleanup Shaders
  cubeShader.cleanup();
  depthShader.cleanup();
  debugShader.cleanup();

  //Cleanup Depthmap Stuff
  glDeleteTextures(1, &depthMap);
  glDeleteFramebuffers(1, &depthMapFBO);

  //Destroy Context and Window
	SDL_GL_DeleteContext( gContext );
	SDL_DestroyWindow( gWindow );

  //Quit SDL subsystems
  TTF_Quit();
  SDL_Quit();
}

void View::loadChunkModels(World &world, Player player){
  //Update the Models for the Chunks

  if(updateLOD){
    //If we have changed LOD, we have to udpate all models.
    models.clear();
  }
  else{
    //Otherwise, only remove the ones where the chunks were also removed.
    while(!world.updateModels.empty()){
      models.erase(models.begin()+world.updateModels.top());
      world.updateModels.pop();
    }
  }

  //Loop over all chunks
  for(unsigned int i = 0; i < world.chunks.size(); i++){
    //If we are at capacity, add a new item
    if(i == models.size()){
      Model model;
      //model.fromOctree(world.chunks[i].data, LOD, glm::vec3(0.0));
      model.fromChunk(world.chunks[i], LOD);
      model.setup();
      models.push_back(model);
    }

    //Old chunks need to be translated too. Translate according to player position.
    glm::vec3 axis = (world.chunks[i].pos)*(float)world.chunkSize-viewPos;

    //Translate the guy
    models[i].reset();
    models[i].translate(axis);
  }
  //Make sure updateLOD is false
  updateLOD = false;
}

bool View::setupShadow(){
  //Setup the QUAD VAO and VBO
  //Generate the quadVAO
  GLfloat vert[] = {-1.0, -1.0, -1.0,
                    -1.0, -0.5, -1.0,
                    -0.5, -1.0, -1.0,
                    -0.5, -0.5, -1.0};

  GLfloat tex[] = {0.0, 0.0,
                   0.0, 1.0,
                   1.0, 0.0,
                   1.0, 1.0};

  //Load it into a buffer thingy
  glGenVertexArrays(1, depthVAO);
  glBindVertexArray(depthVAO[0]);
  glGenBuffers(2, depthVBO);

  //Buff' it
  glBindBuffer(GL_ARRAY_BUFFER, depthVBO[0]);
  glBufferData(GL_ARRAY_BUFFER, 12*sizeof(GLfloat), &vert[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, depthVBO[1]);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), &tex[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

  //Generate Framebuffer and Texture
  glGenFramebuffers(1, &depthMapFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

  //Generate and Bind the Texture
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);

  //Generate an Empty Image Here
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  //Add Parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);

  //Check Success
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;

  return true;
}


void View::render(World world, Player player, Population population){
  //Move the light ayy
  //depthCamera = glm::rotate(depthCamera, glm::radians(-0.1f), glm::vec3(0.0, 1.0, 0.0));
  //Render the Regular View
  renderShadow();
  renderScene();
  renderSprites(world, player, population);

  //Swap the Window
  SDL_GL_SwapWindow(gWindow);
}

void View::renderSprites(World world, Player player, Population population){
  //Render the Sprite
  spriteShader.useProgram();
  glActiveTexture(GL_TEXTURE0);

  //Loop over the guys
  for(unsigned int i = 0; i < population.bots.size(); i++){
    //Here we should check if the sprite should even be rendered.

    //I need to actually check if the sprite is outside the renderdistance!
    if(glm::any(glm::greaterThan(glm::abs(glm::floor(population.bots[i].pos/glm::vec3(world.chunkSize))-glm::floor(viewPos/glm::vec3(world.chunkSize))), renderDistance))){
      //Skip this sprite
      continue;
    }

    //Bind the Appropriate Texture and add it to the shader as a uniform
    glBindTexture(GL_TEXTURE_2D, population.bots[i].sprite.texture);
    spriteShader.setInt("spriteTexture", 0);

    //Set the Position of the Sprite relative to the player
    glm::vec3 a = population.bots[i].pos-viewPos;
    population.bots[i].sprite.model = glm::translate(population.bots[i].sprite.model, glm::vec3(a));
    glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f);
    population.bots[i].sprite.model = glm::rotate(population.bots[i].sprite.model, glm::radians(45.0f), axis);
    population.bots[i].sprite.model = glm::rotate(population.bots[i].sprite.model, glm::radians(-rotation), glm::vec3(0, 1, 0));
    spriteShader.setMat4("mvp", projection*camera*population.bots[i].sprite.model);

    //The Matrix for Transforming the animation thingy
    spriteShader.setFloat("nframe", (float)population.bots[i].sprite.animation.nframe);
    spriteShader.setFloat("animation", (float)population.bots[i].sprite.animation.ID);
    spriteShader.setFloat("width", (float)population.bots[i].sprite.animation.w);
    spriteShader.setFloat("height", (float)population.bots[i].sprite.animation.h);

    //Draw
    glBindVertexArray(population.bots[i].sprite.vao[0]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }
}

void View::renderDepth(){
  //I guess just make a quad
  debugShader.useProgram();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  debugShader.setInt("shadowMap", 0);

  glBindVertexArray(depthVAO[0]);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void View::renderShadow(){
  //Clear the Stuff
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glCullFace(GL_BACK);
  glClear(GL_DEPTH_BUFFER_BIT);

  //Use the Shader
  depthShader.useProgram();
  depthShader.setMat4("dvp", depthProjection * depthCamera * glm::mat4(1.0f));

  //Use the Framebuffer
  glClear(GL_DEPTH_BUFFER_BIT);

  //Activate the Texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  debugShader.setInt("shadowMap", 0);

  //Loop over the Models to Render to Shadowmap
  for(unsigned int i = 0; i < models.size(); i++){
    //Set the Projection Stuff
    depthShader.setMat4("model", models[i].model);
    //Render the Model
    models[i].render();
  }
}

void View::renderScene(){
  //Unbind the Framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Clear the Color and Stuff
  glClearColor(0.6f, 0.9f, 0.8f, 1.0f); //Blue

  //glClearColor(0.25f, 0.6f, 0.4f, 1.0f); //Green
  //glClearColor(0.25f, 0.15f, 0.25f, 1.0f); //Purple
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_BACK);

  glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
	);

  //Use the Shader
  cubeShader.useProgram();    //Use the model's shader
  cubeShader.setVec3("lightCol", lightCol);
  cubeShader.setVec3("lightPos", lightPos);
  //Set the other matrices
  cubeShader.setMat4("projection", projection);
  cubeShader.setMat4("camera", camera);
  cubeShader.setMat4("dbmvp", biasMatrix * depthProjection * depthCamera * glm::mat4(1.0f));
  cubeShader.setMat4("dmvp", depthProjection * depthCamera * glm::mat4(1.0f));

  //Activate and Bind the Texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  cubeShader.setInt("shadowMap", 0);

  //Loop over the Stuff
  for(unsigned int i = 0; i < models.size(); i++){
    //View Projection Matrix
    cubeShader.setMat4("model", models[i].model);
    //Render the Model
    models[i].render();               //Render
  }
}

bool View::switchLOD(World &world, Player &player, int _LOD){
  //Make sure we don't switch the LOD to often
  if(_LOD == LOD || LOD > log2(world.chunkSize)){
    return false;
  }

  //Change the LOD, update the stuff
  player.renderDistance += glm::vec3(2)*glm::vec3(LOD-_LOD);
  LOD = _LOD;
  updateLOD = true;
  world.bufferChunks( *this );
  loadChunkModels(world, player);

  return true;
}

void View::calcFPS(){
  //We getting 60 FPS
  FPS = (int)(1000.0f/(SDL_GetTicks()-ticks));
  ticks = SDL_GetTicks();
  std::cout<<FPS<<std::endl;
}
