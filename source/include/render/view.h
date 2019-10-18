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
  Shader shaderColorPick;
  glm::mat4 model = glm::mat4(1.0f);
  GLuint vbo, vao;

  //Colorings
  glm::vec3 hoverColorBlock = glm::vec3(1.0f);
  glm::vec3 clickColorBlock = glm::vec3(1.0f, 1.0f, 1.0f);

  void setup();
};

void Picker::setup(){
  shaderColorPick.setup("pick/colorPick.vs", "pick/colorPick.fs");
  shaderColorPick.addAttribute(0, "in_Position");

  //Setup the VAO and stuff
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);

  GLfloat cubewire[48] = {   -0.5,  0.5,  0.5,
                             -0.5,  0.5, -0.5,
                              0.5,  0.5, -0.5,
                              0.5,  0.5,  0.5,
                             -0.5,  0.5,  0.5,
                             -0.5, -0.5,  0.5,
                             -0.5, -0.5, -0.5,
                             -0.5,  0.5, -0.5,
                             -0.5, -0.5, -0.5,
                              0.5, -0.5, -0.5,
                              0.5,  0.5, -0.5,
                              0.5, -0.5, -0.5,
                              0.5, -0.5,  0.5,
                              0.5,  0.5,  0.5,
                              0.5, -0.5,  0.5,
                             -0.5, -0.5,  0.5};

  //Based on what is picked, draw an outline.
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 48*sizeof(GLfloat), &cubewire[0], GL_STATIC_DRAW); //3*4
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
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
    bool picked = false;
    glm::vec3 select = glm::vec3(0);

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
    glm::mat4 camera = glm::lookAt(glm::vec3(10,10,10), glm::vec3(0,0,0), glm::vec3(0,1,0));
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

    //Get the Intersection and Stuff
    glm::vec3 intersect(World world, glm::vec2 mouse);

    //FPS Calculator
    void calcFPS();
    int ticks = 0;
    const int plotSize = 100;
    float FPS = 0.0f;
    float arr[100] = {0};
};

glm::vec3 View::intersect(World world, glm::vec2 mouse){
  //Rotation Matrix
  glm::mat4 _rotate = glm::rotate(glm::mat4(1.0), glm::radians(-rotation), glm::vec3(0, 1, 0));
  glm::vec3 _cameraposabs = _rotate*glm::vec4(10.0, 10.0, 10.0, 1.0);
  glm::vec3 _camerapos = viewPos + _cameraposabs;

  //Get our position offset
  float scalex = 2.0f*(mouse.x/SCREEN_WIDTH)-1.0f;
  float scaley = 2.0f*(mouse.y/SCREEN_HEIGHT)-1.0f;

  glm::vec3 _dir =  glm::normalize(glm::vec3(0, 0, 0)-_cameraposabs);
  glm::vec3 _xdir = glm::normalize(glm::vec3(-_dir.z , 0, _dir.x));
  glm::vec3 _ydir = glm::normalize(glm::cross(_dir, _xdir));
  glm::vec3 _startpos = _camerapos + _xdir*scalex*(SCREEN_WIDTH*zoom) + _ydir*scaley*(SCREEN_HEIGHT*zoom);

  float length = 0.0;

  while(world.getBlock(glm::round(_startpos + length * _dir)) == BLOCK_AIR){
    length += 0.8;
  }

  return glm::round(_startpos+length*_dir);
}
