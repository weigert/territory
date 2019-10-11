#include "billboard.h"

bool Billboard::fromRaw(SDL_Surface* TextureImage){
  //Load the File
  if(TextureImage == NULL){
    std::cout<<"Error loading billboard from surface."<<std::endl;
    return false;
  }

  glGenTextures( 1, &texture );
  glBindTexture( GL_TEXTURE_2D, texture );

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureImage->w, TextureImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureImage->pixels);

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  return true;
}

bool Billboard::fromImage(std::string file){
  //Load some image file
  return true;
}

//Do this guy
void Billboard::render(){
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Billboard::renderDepth(){
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthTexture);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}





void Billboard::cleanup(){
  //Cleanup Depthmap Stuff
  glDeleteTextures(1, &texture);
  glDeleteFramebuffers(1, &fbo);
}
