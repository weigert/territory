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

  //Texture Parameters
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  if ( TextureImage )
	    SDL_FreeSurface( TextureImage );

  return true;
}

void Sprite::setupBuffer(bool small){
  GLfloat _small[] = {-0.5, 0.0, 0.0,
                    -0.5, 1.0, 0.0,
                    0.5, 0.0, 0.0,
                    0.5, 1.0, 0.0};

  GLfloat vert[] = {-0.8, 0.0, 0.0,
                    -0.8, 3.2, 0.0,
                    0.8, 0.0, 0.0,
                    0.8, 3.2, 0.0};

  GLfloat tex[] = {0.0, 1.0,
                   0.0, 0.0,
                   1.0, 1.0,
                   1.0, 0.0};

   //Load it into a buffer thingy
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
   glGenBuffers(2, vbo);

   //Buff' it
   glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
   if(small){
     glBufferData(GL_ARRAY_BUFFER, 12*sizeof(GLfloat), &_small[0], GL_STATIC_DRAW);
   }
   else{
     glBufferData(GL_ARRAY_BUFFER, 12*sizeof(GLfloat), &vert[0], GL_STATIC_DRAW);

   }

   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

   glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
   glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), &tex[0], GL_STATIC_DRAW);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void Sprite::resetModel(){
  //Stuff to get the sprite looking right
  model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));
  model = glm::translate(model, animation.translate*glm::vec3(animation.nframe));
}

void Sprite::setAnimation(int id, glm::vec3 _translate){
  //Set the id in Animation
  animation.ID = id;
  animation.nframe = 0;
  animation.translate =_translate;
  resetModel();

  if(id == 0){
    animation.frames = 1;
  }
  else if(id == 1){
    animation.frames = 4;
  }
  else if(id == 2){
    animation.frames = 4;
  }
  else if(id == 3){
    animation.frames = 24;
  }
}

bool Sprite::doAnimationFrame(){
  //Increast the animation frame
  animation.nframe++;
  if(animation.nframe >= animation.frames){
    //Reset our Animation
    setAnimation(0, glm::vec3(0));
    return true;
  }
  return false;
}

void Sprite::render(){
  //Bind our VAO, render it.
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Sprite::cleanup(){
  //Cleanup Depthmap Stuff
  glDeleteTextures(1, &texture);

  //Delete the Stuff
  glDisableVertexAttribArray(vao);
  glDeleteBuffers(2, vbo);
  glDeleteVertexArrays(1, &vao);
}
