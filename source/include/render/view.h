#pragma once
#include "../forward/view.fwd.h"
//Dependencies
#include "../forward/world.fwd.h"
#include "../forward/sprite.fwd.h"
#include "../forward/shader.fwd.h"
#include "../forward/model.fwd.h"
#include "../forward/population.fwd.h"
#include "../forward/bot.fwd.h"
#include "../forward/player.fwd.h"
#include "../forward/interface.fwd.h"

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

    //Viewposition
    glm::vec3 viewPos = glm::vec3(88, 14, 88);
    glm::vec3 renderDistance = glm::vec3(2, 2, 2);

    //LOD Handling
    bool switchLOD(World &world, Player &player, int _LOD);
    int LOD = 4;
    float zoom = 0.01;
    float zoomInc = 0.001;
    bool updateLOD = false;

    //Loaded Chunk Models
    void loadChunkModels(World &world);  //When chunks are displayed
    std::vector<Model> models;

    // Shadow Textures in Here
    bool setupShadow();
    GLuint depthMap;
    GLuint depthMapFBO;
    GLuint depthVAO[1];
    GLuint depthVBO[2];

    //Shaders
    void setupShaders();
    Shader cubeShader;
    Shader depthShader;
    Shader debugShader;
    Shader spriteShader;

    //Render the entire drawing pipeline
    void render(World &world, Player &player, Population &population);
    void renderScene();
    void renderShadow();
    void renderDepth();
    void renderGUI(World &world, Player &player, Population &population);
    void renderSprites(World world, Player player, Population population);

    //View Projector
    glm::mat4 camera = glm::lookAt(glm::vec3(10,12,10), glm::vec3(0,2,0), glm::vec3(0,1,0));
    glm::mat4 projection = glm::ortho(-(float)SCREEN_WIDTH*zoom, (float)SCREEN_WIDTH*zoom, -(float)SCREEN_HEIGHT*zoom, (float)SCREEN_HEIGHT*zoom, -200.0f, 200.0f);
    //glm::mat4 projection = glm::perspective(glm::radians(50.0f), 1200.0f / 800.0f, 0.1f, 100.0f);
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
