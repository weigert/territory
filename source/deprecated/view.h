class View{
  //Renderer
  public:
    //Color Information for now so we can see the pattern
    int d[3] = {0x27333666, 0x4e676c66, 0x66ff9966};

    int ticks= 0;
    int FPS = 0;

    int h = 20;
    int w = 20;

    int renderWidth = 60;
    int renderHeight = 80;

    //Draw a simple triangle grid system
    void render(SDL_Renderer* gRenderer);
    void render2(SDL_Renderer* gRenderer);
    void calcFPS();

};
