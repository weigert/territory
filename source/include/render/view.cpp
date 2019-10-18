//Load Dependencies
#include "../world/world.h"
#include "sprite.h"
#include "shader.h"
#include "model.h"
#include "billboard.h"
#include "interface.h"
#include "../taskbot/population.h"
#include "../taskbot/bot.h"
#include "../game/player.h"
#include "../game/item.h"
//Load our Own Type!
#include "view.h"

/*
================================================================================
                              Setup / Cleanup
================================================================================
*/

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

  //Setup the Guy
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io = ImGui::GetIO(); (void)io;

  ImGui_ImplSDL2_InitForOpenGL(gWindow, gContext);
  ImGui_ImplOpenGL3_Init("#version 130");

  ImGui::StyleColorsCustom();

  //Configure Global OpenGL State
  glEnable( GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
  glEnable(GL_BLEND) ;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
  glLineWidth(2.5f);

  setupShaders();

  //This shoudl now work.
  image.setup();
  shadow.width = SHADOW_WIDTH;
  shadow.height = SHADOW_HEIGHT;
  shadow.setup2();
  temp1.setup();
  temp2.setup();

  picker.setup();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return true;
}

void View::setupShaders(){
  //Setup Cube Shader
  cubeShader.setup("default.vs", "default.fs");
  cubeShader.addAttribute(0, "in_Position");
  cubeShader.addAttribute(1, "in_Color");
  cubeShader.addAttribute(2, "in_Normal");

  //Setup Depthshader
  depthShader.setup("depth.vs", "depth.fs");
  depthShader.addAttribute(0, "in_Position");

  //Setup Spriteshader
  spriteShader.setup("sprite.vs", "sprite.fs");
  spriteShader.addAttribute(0, "in_Quad");
  spriteShader.addAttribute(1, "in_Tex");

  itemShader.setup("item.vs", "item.fs");
  itemShader.addAttribute(0, "in_Quad");
  itemShader.addAttribute(1, "in_Tex");

  //Setup Billboardshader
  blurShader.setup("dof.vs", "dof.fs");
  blurShader.addAttribute(0, "in_Quad");
  blurShader.addAttribute(1, "in_Tex");

  billboardShader.setup("billboard.vs", "billboard.fs");
  billboardShader.addAttribute(0, "in_Quad");
  billboardShader.addAttribute(1, "in_Tex");
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
  spriteShader.cleanup();
  billboardShader.cleanup();
  itemShader.cleanup();
  blurShader.cleanup();

  image.cleanup();
  shadow.cleanup();
  temp1.cleanup();
  temp2.cleanup();

  //Shutdown IMGUI
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  //Destroy Context and Window
	SDL_GL_DeleteContext( gContext );
	SDL_DestroyWindow( gWindow );

  //Quit SDL subsystems
  TTF_Quit();
  SDL_Quit();
}

/*
================================================================================
                              Model Generation
================================================================================
*/

void View::loadChunkModels(World &world){
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

void View::updateChunkModels(World &world){
  //Loop over all chunks, see if they are updated.
  for(unsigned int i = 0; i < world.chunks.size(); i++){
    if(world.chunks[i].refreshModel){
      models[i].fromChunk(world.chunks[i], LOD);
      models[i].update();
      world.chunks[i].refreshModel = false;
    }
  }
}

/*
================================================================================
                                Rendering
================================================================================
*/

void View::update(){
  /*
  lightPos += glm::vec3(-0.01f, 0.0f, -0.01f);
  depthCamera = glm::lookAt(lightPos, glm::vec3(0,0,0), glm::vec3(0,1,0));
  */
}

void View::render(World &world, Player &player, Population &population){
/*
SHADOW MAPPING
*/
  //-- Screen: Shadow FBO, No Filter
  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow.fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //Use the Shader
  depthShader.useProgram();
  depthShader.setMat4("dvp", depthProjection * depthCamera * glm::mat4(1.0f));

  //Activate the Texture
  glClear(GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, shadow.depthTexture);

  //Loop over the Models to Render to Shadowmap
  for(unsigned int i = 0; i < models.size(); i++){
    //Set the Projection Stuff
    depthShader.setMat4("model", models[i].model);
    //Render the Model
    models[i].render();
  }
  glBindVertexArray(0);

/*
REGULAR IMAGE
*/

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  //Render entire scene to the actual image!
  glBindFramebuffer(GL_FRAMEBUFFER, image.fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  //Clear the Color and Stuff
  glClearColor(skyCol.x, skyCol.y, skyCol.z, 1.0f); //Blue
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glCullFace(GL_BACK);

  glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
	);

  //Use the Shader
  cubeShader.useProgram();    //Use the model's shader

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, shadow.depthTexture);
  cubeShader.setInt("shadowMap", 0);
  cubeShader.setVec3("lightCol", lightCol);
  cubeShader.setVec3("lightPos", lightPos);
  //Set the other matrices
  cubeShader.setMat4("projection", projection);
  cubeShader.setMat4("camera", camera);
  cubeShader.setMat4("dbmvp", biasMatrix * depthProjection * depthCamera * glm::mat4(1.0f));
  cubeShader.setMat4("dmvp", depthProjection * depthCamera * glm::mat4(1.0f));

  //Activate and Bind the Texture

  //Loop over the Stuff
  for(unsigned int i = 0; i < models.size(); i++){
    //View Projection Matrix
    cubeShader.setMat4("model", models[i].model);
    //Render the Model
    models[i].render();               //Render
  }

  /* Picker Cube! */
  if(picked){
    picker.shaderColorPick.useProgram();
    picker.model = glm::mat4(1.0f);
    glm::vec3 a = select-viewPos;
    picker.model = glm::translate(picker.model, a);
    picker.shaderColorPick.setVec3("un_Color", picker.clickColorBlock);
    picker.shaderColorPick.setMat4("mvp", projection*camera*picker.model);
    glBindVertexArray(picker.vao);
    glDrawArrays(GL_LINE_STRIP, 0, 16);
  }

  /* Character Sprites */
  spriteShader.useProgram();
  //Loop over all Sprites
  for(unsigned int i = 0; i < population.bots.size(); i++){
    //Here we should check if the sprite should even be rendered.
    if(population.bots[i].dead) continue;
    if(glm::any(glm::greaterThan(glm::abs(glm::floor(population.bots[i].pos/glm::vec3(world.chunkSize))-glm::floor(viewPos/glm::vec3(world.chunkSize))), renderDistance)))
      continue;

    population.bots[i].sprite.resetModel();
    //Set the Position of the Sprite relative to the player
    glm::vec3 a = population.bots[i].pos-viewPos;
    population.bots[i].sprite.model = glm::translate(population.bots[i].sprite.model, a);
    glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f);
    population.bots[i].sprite.model = glm::rotate(population.bots[i].sprite.model, glm::radians(45.0f), axis);
    population.bots[i].sprite.model = glm::rotate(population.bots[i].sprite.model, glm::radians(-rotation), glm::vec3(0, 1, 0));

    //Setup the Shader
    spriteShader.setInt("spriteTexture", 0);
    spriteShader.setMat4("mvp", projection*camera*population.bots[i].sprite.model);
    spriteShader.setFloat("nframe", (float)population.bots[i].sprite.animation.nframe);
    spriteShader.setFloat("animation", (float)population.bots[i].sprite.animation.ID);
    spriteShader.setFloat("width", (float)population.bots[i].sprite.animation.w);
    spriteShader.setFloat("height", (float)population.bots[i].sprite.animation.h);

    //Draw
    population.bots[i].sprite.render();
  }

  /* Item Sprites */
  itemShader.useProgram();
  //Loop over all Sprites
  for(unsigned int i = 0; i < world.drops.size(); i++){
    //Here we should check if the sprite should even be rendered.
    if(glm::any(glm::greaterThan(glm::abs(glm::floor(world.drops[i].pos/glm::vec3(world.chunkSize))-glm::floor(viewPos/glm::vec3(world.chunkSize))), renderDistance)))
      continue;

    //Set the Position of the Sprite relative to the player
    world.drops[i].sprite.resetModel();
    glm::vec3 a = world.drops[i].pos-viewPos;
    world.drops[i].sprite.model = glm::translate(world.drops[i].sprite.model, glm::vec3(a));
    glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f);
    world.drops[i].sprite.model = glm::rotate(world.drops[i].sprite.model, glm::radians(45.0f), axis);
    world.drops[i].sprite.model = glm::rotate(world.drops[i].sprite.model, glm::radians(-rotation), glm::vec3(0, 1, 0));

    //Setup the Shader
    itemShader.setInt("spriteTexture", 0);
    itemShader.setFloat("index", (float)world.drops[i]._type);
    itemShader.setMat4("mvp", projection*camera*world.drops[i].sprite.model);

    //Draw the drops!
    world.drops[i].sprite.render();
  }

/*
AFTER-EFFECTS
*/

  //Render Temp to Screen with a horizontal blur shader
  glBindFramebuffer(GL_FRAMEBUFFER, temp1.fbo);
  blurShader.useProgram();
  blurShader.setFloat("mousex", focus.x);
  blurShader.setFloat("mousey", focus.y);
  blurShader.setFloat("width", SCREEN_WIDTH);
  blurShader.setFloat("height", SCREEN_HEIGHT);
  blurShader.setBool("vert", false);
  glActiveTexture(GL_TEXTURE0+0);
  glBindTexture(GL_TEXTURE_2D, image.texture);
  blurShader.setInt("imageTexture", 0);
  glActiveTexture(GL_TEXTURE0+1);
  glBindTexture(GL_TEXTURE_2D, image.depthTexture);
  blurShader.setInt("depthTexture", 1);
  glBindVertexArray(image.vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  //Render screen to monitor with vertical blur shader
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glActiveTexture(GL_TEXTURE0+0);
  glBindTexture(GL_TEXTURE_2D, temp1.texture);
  blurShader.setInt("imageTexture", 0);
  glActiveTexture(GL_TEXTURE0+1);
  glBindTexture(GL_TEXTURE_2D, image.depthTexture);
  blurShader.setInt("depthTexture", 1);
  blurShader.setBool("vert", true);
  glBindVertexArray(temp1.vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  //Add the GUI
  renderGUI(world, player, population);

  //Swap the Window
  SDL_GL_SwapWindow(gWindow);
}

//User Interface
void View::renderGUI(World &world, Player &player, Population &population){
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(gWindow);
  ImGui::NewFrame();

  //Draw to ImGui
  interface->render(*this, world, population, player);

  //Draw the cool window
  ImGui::ShowDemoWindow();

  //Render IMGUI
  ImGui::Render();
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/*
================================================================================
                                    Helpers
================================================================================
*/

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
  loadChunkModels(world);

  return true;
}

void View::calcFPS(){
  //Loop over the FPS
  //We getting 60 FPS
  FPS = (int)(1000.0f/(SDL_GetTicks()-ticks));
  ticks = SDL_GetTicks();
  //Set the FPS
  for(int i = 0; i < plotSize-1; i++){
    arr[i] = arr[i+1];
  }
  arr[plotSize-1] = FPS;
}
