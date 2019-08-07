#include "sprite.h"

bool Sprite::loadImage(std::string fileName){
  //Get the Path
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "resource/sprite";

  //Load the Image to a surface
  SDL_Surface *TextureImage = IMG_Load((data_dir/fileName).c_str());

  //Load the File
  if(TextureImage == NULL){
    std::cout<<"Error loading sprite."<<std::endl;
    return false;
  }

  //Generate the Texture
  glGenTextures( 1, &texture );
  glBindTexture( GL_TEXTURE_2D, texture );

  //Load the Image to the Texture
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, TextureImage->w, TextureImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureImage->pixels );
  glGenerateMipmap(GL_TEXTURE_2D);

  //Texture Parameters
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  if ( TextureImage )
	    SDL_FreeSurface( TextureImage );

  return true;
}

void Sprite::setupBuffer(){
/*
Bottom Left
Top Left
Top Right
Bottom Right
*/

  GLfloat vert[] = {-0.8, 0.0, 0.0,
                    -0.8, 3.2, 0.0,
                    0.8, 0.0, 0.0,
                    0.8, 3.2, 0.0};

  GLfloat tex[] = {0.0, 1.0,
                   0.0, 0.0,
                   1.0, 1.0,
                   1.0, 0.0};

   //Load it into a buffer thingy
   glGenVertexArrays(1, vao);
   glBindVertexArray(vao[0]);
   glGenBuffers(2, vbo);

   //Buff' it
   glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
   glBufferData(GL_ARRAY_BUFFER, 12*sizeof(GLfloat), &vert[0], GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

   glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
   glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), &tex[0], GL_STATIC_DRAW);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void Sprite::cleanup(){
  //Cleanup Depthmap Stuff
  glDeleteTextures(1, &texture);

  //Delete the Stuff
  glDisableVertexAttribArray(vao[0]);
  glDeleteBuffers(2, vbo);
  glDeleteVertexArrays(1, vao);
}
