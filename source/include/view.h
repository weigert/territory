class View{
  public:
    //Initialization
    bool Init();
    void cleanup();
    SDL_Window* gWindow;
    SDL_GLContext gContext;
    const unsigned int SCREEN_WIDTH = 1200, SCREEN_HEIGHT = 800;
    const unsigned int SHADOW_WIDTH = 2000, SHADOW_HEIGHT = 2000;
    float zoom = 0.01;
    float zoomInc = 0.001;
    int LOD = 4;
    bool updateLOD = false;
    float rotation = 0.0;
    int renderDistance = 1;
    bool mapView = false;
    bool debug = false;

    //Loaded Chunk Models
    std::vector<Model> models;
    void loadChunkModels(World &world);  //When chunks are displayed

    // Shadow Textures in Here
    GLuint depthMap;
    GLuint depthMapFBO;

    //Thingy
    GLuint depthVAO[1];
    GLuint depthVBO[2];

    bool setupShadow();

    Sprite sprite;

    //Shaders
    void setupShaders();
    Shader cubeShader;
    Shader depthShader;
    Shader debugShader;
    Shader spriteShader;

    //Render the entire drawing pipeline
    void render(World world);
    void renderScene();
    void renderShadow();
    void renderDepth();
    void renderSprite();

    //View Projector
    glm::mat4 camera = glm::lookAt(glm::vec3(10,12,10), glm::vec3(0,2,0), glm::vec3(0,1,0));
    glm::mat4 projection = glm::ortho(-(float)SCREEN_WIDTH*zoom, (float)SCREEN_WIDTH*zoom, -(float)SCREEN_HEIGHT*zoom, (float)SCREEN_HEIGHT*zoom, -100.0f, 100.0f);
    //glm::mat4 projection = glm::perspective(glm::radians(50.0f), 1200.0f / 800.0f, 0.1f, 100.0f);
    //Light Variables
    glm::vec3 lightPos = glm::vec3(3.0f, 6.0f, 2.0f);
    glm::vec3 lightCol = glm::vec3(1.0f, 1.0f, 0.9f);
    //glm::vec3 lightCol = glm::vec3(0.15f, 0.05f, 0.15f); //Purple

    glm::mat4 depthModelMatrix = glm::mat4(1.0);
    glm::mat4 depthProjection = glm::ortho<float>(-60,60,-60,60,-30,100);
    glm::mat4 depthCamera = glm::lookAt(lightPos, glm::vec3(0,0,0), glm::vec3(0,1,0));

    //FPS Calculator
    void calcFPS();
    int ticks = 0;
    float FPS = 0;
};
