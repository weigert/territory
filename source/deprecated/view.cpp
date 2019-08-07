#include "view.h"

//Define the Member Functions of the View Class

void View::render(SDL_Renderer* gRenderer){

  SDL_SetRenderDrawColor(gRenderer, 100, 200, 100, 100);

  //Drawing Loop for the Triangle Bands
  for(int i = 0; i < renderHeight+1; i++){
    for(int j = 0; j < renderWidth; j++){
      //Coordinates
      int a[2] = {j*w+w/2+ (((((j+renderWidth-1)%2)+(i%2))%2)*2-1)*w/2, (i-1)*h/2};
      int b[2] = {j*w+w/2+ (((((j)%2)+(i%2))%2)*2-1)*w/2,  i*h/2};
      int c[2] = {j*w+w/2+ (((((j+1)%2)+(i%2))%2)*2-1)*w/2, (i+1)*h/2};

      //Full surroundings
      filledTrigonColor(gRenderer, a[0], a[1], b[0], b[1], c[0], c[1], d[(i+j)%3]);
      trigonRGBA(gRenderer, a[0], a[1], b[0], b[1], c[0], c[1], 200, 200, 200, 100);

    //  SDL_RenderDrawLine(gRenderer, 10*i, 10*j, 10*i+10, 10*j+10);
      //SDL_RenderDrawLine(gRenderer, 10*i, 10*j, 10*i+10, 10*j+10);
    }
  }
}

void View::render2(SDL_Renderer* gRenderer){

  SDL_SetRenderDrawColor(gRenderer, 100, 200, 100, 100);
  int i = 0;
  int j = 0;

  int a[2] = {j*w+w/2 + (((((j+renderWidth-1)%2)+(i%2))%2)*2-1)*w/2, (i-1)*h/2};
  int b[2] = {j*w+w/2 + (((((j)%2)+(i%2))%2)*2-1)*w/2,  i*h/2};
  int c[2] = {j*w+w/2 + (((((j+1)%2)+(i%2))%2)*2-1)*w/2, (i+1)*h/2};

  //Drawing Loop for the Triangle Bands
  for(j = 0; j < renderWidth; j++){
    for(i = 0; i < renderHeight+1; i++){
      //Coordinates

      //Full surroundings
      filledTrigonColor(gRenderer, a[0], a[1], b[0], b[1], c[0], c[1], d[(i+j)%3]);
      //trigonRGBA(gRenderer, a[0], a[1], b[0], b[1], c[0], c[1], 200, 200, 200, 100);

      //Update Coordinates
      a[0] = b[0];
      a[1] = (i-1)*h/2;
      b[0] = c[0];
      b[1] = i*h/2;
      c[0] = j*w+w/2 + (((((j+1)%2)+(i%2))%2)*2-1)*w/2;
      c[1] = (i+1)*h/2;


    //  SDL_RenderDrawLine(gRenderer, 10*i, 10*j, 10*i+10, 10*j+10);
      //SDL_RenderDrawLine(gRenderer, 10*i, 10*j, 10*i+10, 10*j+10);
    }
  }
}


void View::calcFPS(){
  FPS = 1000/(SDL_GetTicks()-ticks);
  ticks = SDL_GetTicks();
  std::cout<<FPS<<std::endl;
}
