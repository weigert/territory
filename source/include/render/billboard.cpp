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



//Generate all the Data for this Billboard (which can be rendered to in 3D in theory)
bool Billboard::setup(int width, int height){
  //Generate Framebuffer and Texture
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  //Generate and Bind the Texture
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

  //Generate and Bind the Texture
  glGenTextures(1, &depthTexture);
  glBindTexture(GL_TEXTURE_2D, depthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;

  //Reset
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Setup the VAO and stuff
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLfloat vert[] = {-1.0, -1.0,
                    -1.0, 1.0,
                    1.0, -1.0,
                    1.0, 1.0};

  GLfloat tex[] = {0.0, 0.0,
                   0.0, 1.0,
                   1.0, 0.0,
                   1.0, 1.0};

  glGenBuffers(2, &vbo[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), &vert[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), &tex[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

  return true;
}

bool Billboard::setup2(int width, int height){
  //Generate Framebuffer and Texture
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  //Generate Texture
  glGenTextures(1, &depthTexture);
  glBindTexture(GL_TEXTURE_2D, depthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

  /* NEW */
  glDrawBuffer(GL_NONE);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;

  //Setup the QUAD VAO and VBO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  //Generate the quadVAO
  GLfloat vert[] = {-1.0, -1.0,
                    -1.0, 1.0,
                    1.0, -1.0,
                    1.0, 1.0};

  GLfloat tex[] = {0.0, 0.0,
                   0.0, 1.0,
                   1.0, 0.0,
                   1.0, 1.0};

  glGenBuffers(2, &vbo[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), &vert[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), &tex[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

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
