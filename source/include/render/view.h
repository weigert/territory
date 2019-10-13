#pragma once
#include "../forward/view.fwd.h"
//Dependencies
#include "../forward/billboard.fwd.h"
#include "../forward/world.fwd.h"
#include "../forward/sprite.fwd.h"
#include "../forward/shader.fwd.h"
#include "../forward/model.fwd.h"
#include "../forward/population.fwd.h"
#include "../forward/bot.fwd.h"
#include "../forward/player.fwd.h"
#include "../forward/interface.fwd.h"
#include "../forward/item.fwd.h"


class Picker{
public:
  Shader shaderPickSprite;
  Shader shaderPickBlock;
  Shader shaderColorPick;

  Billboard imageSprite;
  Billboard imageBlock;

  //Pick Model
  std::vector<GLfloat> positions;
  glm::mat4 model = glm::mat4(1.0f);
  GLuint vbo, vao;

  //Colorings
  glm::vec3 hoverColorBlock = glm::vec3(1.0f);
  glm::vec3 clickColorBlock = glm::vec3(0.5f);
  glm::vec3 hoverColorSprite = glm::vec3(1.0f);
  glm::vec3 clickColorSprite = glm::vec3(0.5f);

  //Actual Pick IDs
  glm::vec3 pickBlock = glm::vec3(0);
  glm::vec3 pickSprite = glm::vec3(0);
  bool block = false;
  bool sprite = false;
  bool picked = false;

  void setup();
  void pick(glm::vec2 mouse);
  void buffer();
  glm::vec3 sample(Billboard image, glm::vec2 mouse);
  void outline();
};

void Picker::setup(){
  shaderPickSprite.setup("pick/picker.vs", "pick/picker.fs");
  shaderPickSprite.addAttribute(0, "in_Position");

  shaderPickBlock.setup("pick/picker.vs", "pick/picker.fs");
  shaderPickBlock.addAttribute(0, "in_Position");

  shaderColorPick.setup("pick/colorPick.vs", "pick/colorPick.fs");
  shaderColorPick.addAttribute(0, "in_Position");

  imageBlock.setup();
  imageSprite.setup();

  //Setup the VAO and stuff
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
}

void Picker::outline(){
  if(block || sprite){
    //Do a render pass with shaderColorPick
    shaderColorPick.useProgram();
    shaderColorPick.setVec3("un_Color", hoverColorBlock);

    //Draw the Lines
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, positions.size()/3);
  }
}

void Picker::buffer(){
  //Generate the Positions Data
  GLfloat cubewire[30] = {  -0.5, -0.5, -0.5,
                            -0.5,  0.5, -0.5,
                             0.5,  0.5, -0.5,
                             0.5, -0.5, -0.5,
                            -0.5, -0.5, -0.5,
                            -0.5, -0.5,  0.5,
                            -0.5,  0.5,  0.5,
                             0.5,  0.5,  0.5,
                             0.5, -0.5,  0.5,
                            -0.5, -0.5,  0.5};

  //Based on what is picked, draw an outline.
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 30*sizeof(GLfloat), &cubewire[0], GL_STATIC_DRAW); //3*4
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Picker::pick(glm::vec2 mouse){
  //Extract the Color from the Texture at mouse position
  glm::vec3 pickBlock = sample(imageBlock, mouse);
  glm::vec3 pickSprite = sample(imageSprite, mouse);

  //Set the "found" flags
  sprite = (glm::all(glm::equal(pickSprite, glm::vec3(0))))?false:true;
  block = (glm::all(glm::equal(pickBlock, glm::vec3(0))))?false:true;

  //Create the outline object
  if(block || sprite){
    buffer();
    picked = true;
  }
  else{
    picked = false;
  }
}

glm::vec3 Picker::sample(Billboard image, glm::vec2 mouse){
  glBindFramebuffer(GL_FRAMEBUFFER, image.fbo);
  glBindTexture (GL_TEXTURE_2D, image.texture);
  unsigned char res[4];
  glReadPixels(mouse.x, mouse.y, 1,1, GL_BGRA, GL_UNSIGNED_BYTE, &res);
  return glm::vec3((int)res[0], (int)res[1], (int)res[2]);
}

class View{
  public:
    //Initialization
    bool Init();
    void cleanup();
    SDL_Window* gWindow;
    SDL_GLContext gContext;
    ImGuiIO io;
    const unsigned int SCREEN_WIDTH = 1200, SCREEN_HEIGHT = 800;
    const unsigned int SHADOW_WIDTH = 2000, SHADOW_HEIGHT = 2000;

    //GUI Handler
    Interface* interface;
    Picker picker;

    //Viewposition
    glm::vec3 viewPos = glm::vec3(30, 0, 30);
    glm::vec3 renderDistance = glm::vec3(3, 3, 3);

    //LOD Handling
    bool switchLOD(World &world, Player &player, int _LOD);
    int LOD = 4;
    float zoom = 0.01;
    float zoomInc = 0.001;
    bool updateLOD = false;
    glm::vec2 focus = glm::vec2(0.1, 0.9);

    //Loaded Chunk Models
    void loadChunkModels(World &world);  //When chunks are displayed
    void updateChunkModels(World &world);  //When chunks are updated
    std::vector<Model> models;

    //Rendering Targets
    Billboard shadow; //Shadowmap to render to
    Billboard image;  //Image as projected normally - no changes!
    Billboard temp1;  //Switch between render passes, 1
    Billboard temp2;  //Switch between render passes, 2

    //Shaders
    void setupShaders();
    Shader cubeShader;
    Shader depthShader;
    Shader spriteShader;
    Shader billboardShader;
    Shader blurShader;
    Shader itemShader;

    //Render the entire drawing pipeline
    void update();
    void render(World &world, Player &player, Population &population);
    void renderGUI(World &world, Player &player, Population &population);

    //View Projector (bunch of camera settings here tbh)
    glm::mat4 camera = glm::lookAt(glm::vec3(10,12,10), glm::vec3(0,2,0), glm::vec3(0,1,0));
    glm::mat4 projection = glm::ortho(-(float)SCREEN_WIDTH*zoom, (float)SCREEN_WIDTH*zoom, -(float)SCREEN_HEIGHT*zoom, (float)SCREEN_HEIGHT*zoom, -200.0f, 200.0f);
    //Light Variables
    glm::vec3 lightPos = glm::vec3(3.0f, 6.0f, 2.0f);
    glm::vec3 lightCol = glm::vec3(1.0f, 1.0f, 0.9f);
    glm::vec3 skyCol = glm::vec3(0.6, 0.9f, 0.8f);
    //glm::vec3 lightCol = glm::vec3(0.15f, 0.05f, 0.15f); //Purple
    float rotation = 0.0;
    glm::mat4 depthModelMatrix = glm::mat4(1.0);
    glm::mat4 depthProjection = glm::ortho<float>(-80,80,-80,80,-30,100);
    glm::mat4 depthCamera = glm::lookAt(lightPos, glm::vec3(0,0,0), glm::vec3(0,1,0));

    //FPS Calculator
    void calcFPS();
    int ticks = 0;
    const int plotSize = 100;
    float FPS = 0.0f;
    float arr[100] = {0};
};
